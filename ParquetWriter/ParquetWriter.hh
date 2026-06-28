#ifndef COLA_PARQUET_WRITER_HH
#define COLA_PARQUET_WRITER_HH

#include <COLA.hh>
#include <arrow/api.h>
#include <arrow/io/file.h>
#include <parquet/arrow/writer.h>

#include <cstdint>
#include <memory>
#include <string>

namespace cola_parquet_writer {

  struct ParquetWriterConfig {
    std::string path = "cola_output.parquet";
    std::string compression = "zstd";
    int64_t batch_size = 1000;
  };

  class ParquetWriter : public cola::VWriter {
   public:
    static inline const std::string kName = "ParquetWriter";

    explicit ParquetWriter(ParquetWriterConfig config);
    ParquetWriter(const ParquetWriter&) = delete;
    ParquetWriter(ParquetWriter&&) = delete;
    ParquetWriter& operator=(const ParquetWriter&) = delete;
    ParquetWriter& operator=(ParquetWriter&&) = delete;
    ~ParquetWriter() override;

    void operator()(std::unique_ptr<cola::EventData>&& data) override;

   private:
    void Flush();
    void AppendEvent(const cola::EventData& data);
    void EnsureFileWriterOpen();

    ParquetWriterConfig config_;
    std::unique_ptr<arrow::RecordBatchBuilder> batch_builder_;
    std::shared_ptr<arrow::io::FileOutputStream> out_stream_;
    std::unique_ptr<parquet::arrow::FileWriter> file_writer_;
    int64_t event_counter_ = 0;
    int64_t buffered_ = 0;
  };

}  // namespace cola_parquet_writer

#endif  // COLA_PARQUET_WRITER_HH
