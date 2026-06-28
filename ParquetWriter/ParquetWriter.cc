#include "ParquetWriter.hh"

#include <iostream>

namespace cola_parquet_writer {

  namespace {

    const std::shared_ptr<arrow::DataType>& ParticleStruct() {
      static const std::shared_ptr<arrow::DataType> type = arrow::struct_({
          // LorentzVector position
          arrow::field("pos_t", arrow::float64()),
          arrow::field("pos_x", arrow::float64()),
          arrow::field("pos_y", arrow::float64()),
          arrow::field("pos_z", arrow::float64()),
          // LorentzVector momentum
          arrow::field("mom_e", arrow::float64()),
          arrow::field("mom_x", arrow::float64()),
          arrow::field("mom_y", arrow::float64()),
          arrow::field("mom_z", arrow::float64()),
          // PDG code
          arrow::field("pdg_code", arrow::int32()),
          // ParticleClass
          arrow::field("p_class", arrow::int8()),
      });
      return type;
    }

    const std::shared_ptr<arrow::Schema>& EventSchema() {
      static const std::shared_ptr<arrow::Schema> schema = arrow::schema({
          // EventIniState
          arrow::field("event_id", arrow::int64()),
          arrow::field("pdg_code_a", arrow::int32()),
          arrow::field("pdg_code_b", arrow::int32()),
          arrow::field("pz_a", arrow::float64()),
          arrow::field("pz_b", arrow::float64()),
          arrow::field("energy", arrow::float64()),
          arrow::field("sect_nn", arrow::float32()),
          arrow::field("b", arrow::float32()),
          arrow::field("num_coll", arrow::int32()),
          arrow::field("num_coll_pp", arrow::int32()),
          arrow::field("num_coll_pn", arrow::int32()),
          arrow::field("num_coll_nn", arrow::int32()),
          arrow::field("num_part", arrow::int32()),
          arrow::field("num_part_a", arrow::int32()),
          arrow::field("num_part_b", arrow::int32()),
          arrow::field("phi_rot_a", arrow::float32()),
          arrow::field("theta_rot_a", arrow::float32()),
          arrow::field("phi_rot_b", arrow::float32()),
          arrow::field("theta_rot_b", arrow::float32()),
          arrow::field("ini_state_particles", arrow::list(ParticleStruct())),
          // EventParticles
          arrow::field("particles", arrow::list(ParticleStruct())),
      });
      return schema;
    }

    void ThrowIfNotOk(const arrow::Status& status) {
      if (!status.ok()) {
        throw std::runtime_error("ParquetWriter Arrow error: " + status.ToString());
      }
    }

    void AppendParticleList(arrow::ListBuilder* list_builder, const cola::EventParticles& particles) {
      ThrowIfNotOk(list_builder->Append());
      auto* struct_builder = static_cast<arrow::StructBuilder*>(list_builder->value_builder());
      for (const auto& particle : particles) {
        ThrowIfNotOk(struct_builder->Append());
        ThrowIfNotOk(static_cast<arrow::DoubleBuilder*>(struct_builder->field_builder(0))->Append(particle.position.t));
        ThrowIfNotOk(static_cast<arrow::DoubleBuilder*>(struct_builder->field_builder(1))->Append(particle.position.x));
        ThrowIfNotOk(static_cast<arrow::DoubleBuilder*>(struct_builder->field_builder(2))->Append(particle.position.y));
        ThrowIfNotOk(static_cast<arrow::DoubleBuilder*>(struct_builder->field_builder(3))->Append(particle.position.z));
        ThrowIfNotOk(static_cast<arrow::DoubleBuilder*>(struct_builder->field_builder(4))->Append(particle.momentum.e));
        ThrowIfNotOk(static_cast<arrow::DoubleBuilder*>(struct_builder->field_builder(5))->Append(particle.momentum.x));
        ThrowIfNotOk(static_cast<arrow::DoubleBuilder*>(struct_builder->field_builder(6))->Append(particle.momentum.y));
        ThrowIfNotOk(static_cast<arrow::DoubleBuilder*>(struct_builder->field_builder(7))->Append(particle.momentum.z));
        ThrowIfNotOk(static_cast<arrow::Int32Builder*>(struct_builder->field_builder(8))->Append(particle.pdg_code));
        ThrowIfNotOk(static_cast<arrow::Int8Builder*>(struct_builder->field_builder(9))
                         ->Append(static_cast<int8_t>(particle.p_class)));
      }
    }

    arrow::Compression::type ParseCompression(const std::string& name) {
      if (name == "zstd") {
        return arrow::Compression::ZSTD;
      }
      if (name == "snappy") {
        return arrow::Compression::SNAPPY;
      }
      if (name == "gzip") {
        return arrow::Compression::GZIP;
      }
      if (name == "none") {
        return arrow::Compression::UNCOMPRESSED;
      }
      throw std::invalid_argument("Unknown compression: " + name);
    }

  }  // namespace

  ParquetWriter::ParquetWriter(ParquetWriterConfig config) : config_(std::move(config)) {
    auto batch_builder = arrow::RecordBatchBuilder::Make(EventSchema(), arrow::default_memory_pool());
    ThrowIfNotOk(batch_builder.status());
    batch_builder_ = std::move(*batch_builder);
  }

  void ParquetWriter::Flush() {
    if (buffered_ == 0) {
      return;
    }
    auto batch = batch_builder_->Flush();
    ThrowIfNotOk(batch.status());
    ThrowIfNotOk(file_writer_->WriteRecordBatch(**batch));
    buffered_ = 0;
  }

  void ParquetWriter::AppendEvent(const EventData& data) {
    const auto& ini = data.ini_state;

    ThrowIfNotOk(batch_builder_->GetFieldAs<arrow::Int64Builder>(0)->Append(event_counter_++));
    ThrowIfNotOk(batch_builder_->GetFieldAs<arrow::Int32Builder>(1)->Append(ini.pdg_code_a));
    ThrowIfNotOk(batch_builder_->GetFieldAs<arrow::Int32Builder>(2)->Append(ini.pdg_code_b));
    ThrowIfNotOk(batch_builder_->GetFieldAs<arrow::DoubleBuilder>(3)->Append(ini.pz_a));
    ThrowIfNotOk(batch_builder_->GetFieldAs<arrow::DoubleBuilder>(4)->Append(ini.pz_b));
    ThrowIfNotOk(batch_builder_->GetFieldAs<arrow::DoubleBuilder>(5)->Append(ini.energy));
    ThrowIfNotOk(batch_builder_->GetFieldAs<arrow::FloatBuilder>(6)->Append(ini.sect_nn));
    ThrowIfNotOk(batch_builder_->GetFieldAs<arrow::FloatBuilder>(7)->Append(ini.b));
    ThrowIfNotOk(batch_builder_->GetFieldAs<arrow::Int32Builder>(8)->Append(ini.num_coll));
    ThrowIfNotOk(batch_builder_->GetFieldAs<arrow::Int32Builder>(9)->Append(ini.num_coll_pp));
    ThrowIfNotOk(batch_builder_->GetFieldAs<arrow::Int32Builder>(10)->Append(ini.num_coll_pn));
    ThrowIfNotOk(batch_builder_->GetFieldAs<arrow::Int32Builder>(11)->Append(ini.num_coll_nn));
    ThrowIfNotOk(batch_builder_->GetFieldAs<arrow::Int32Builder>(12)->Append(ini.num_part));
    ThrowIfNotOk(batch_builder_->GetFieldAs<arrow::Int32Builder>(13)->Append(ini.num_part_a));
    ThrowIfNotOk(batch_builder_->GetFieldAs<arrow::Int32Builder>(14)->Append(ini.num_part_b));
    ThrowIfNotOk(batch_builder_->GetFieldAs<arrow::FloatBuilder>(15)->Append(ini.phi_rot_a));
    ThrowIfNotOk(batch_builder_->GetFieldAs<arrow::FloatBuilder>(16)->Append(ini.theta_rot_a));
    ThrowIfNotOk(batch_builder_->GetFieldAs<arrow::FloatBuilder>(17)->Append(ini.phi_rot_b));
    ThrowIfNotOk(batch_builder_->GetFieldAs<arrow::FloatBuilder>(18)->Append(ini.theta_rot_b));

    AppendParticleList(batch_builder_->GetFieldAs<arrow::ListBuilder>(19), ini.ini_state_particles);
    AppendParticleList(batch_builder_->GetFieldAs<arrow::ListBuilder>(20), data.particles);

    ++buffered_;
  }

  void ParquetWriter::EnsureFileWriterOpen() {
    if (file_writer_) {
      return;
    }

    auto sink = arrow::io::FileOutputStream::Open(config_.path);
    if (!sink.ok()) {
      throw std::runtime_error(sink.status().ToString());
    }

    out_stream_ = *sink;
    auto props = parquet::WriterProperties::Builder().compression(ParseCompression(config_.compression))->build();
    auto arrow_props = parquet::ArrowWriterProperties::Builder().store_schema()->build();
    auto writer =
        parquet::arrow::FileWriter::Open(*EventSchema(), arrow::default_memory_pool(), out_stream_, props, arrow_props);
    if (!writer.ok()) {
      throw std::runtime_error(writer.status().ToString());
    }
    file_writer_ = std::move(*writer);
  }

  void ParquetWriter::operator()(std::unique_ptr<EventData>&& data) {
    EnsureFileWriterOpen();
    AppendEvent(*data);
    if (buffered_ >= config_.batch_size) {
      Flush();
    }
  }

  ParquetWriter::~ParquetWriter() {
    try {
      if (file_writer_) {
        Flush();
        ThrowIfNotOk(file_writer_->Close());
      }
    } catch (...) {
      std::cerr << "ParquetWriter: error during finalization\n";
    }
  }

}  // namespace cola_parquet_writer
