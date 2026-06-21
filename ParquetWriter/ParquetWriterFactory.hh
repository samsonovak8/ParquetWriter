#ifndef COLA_PARQUET_WRITER_PARQUETWRITERFACTORY_HH
#define COLA_PARQUET_WRITER_PARQUETWRITERFACTORY_HH

#include "ParquetWriter.hh"

#include <COLA.hh>

#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_map>

namespace cola_parquet_writer {

  class ParquetWriterFactory : public cola::VWriterFactory {
   public:
    std::unique_ptr<cola::VFilter> Create(const std::unordered_map<std::string, std::string>& meta_data) override {
      ParquetWriterConfig config;
      if (auto it = meta_data.find("path"); it != meta_data.end()) {
        config.path = it->second;
      }
      if (auto it = meta_data.find("compression"); it != meta_data.end()) {
        config.compression = it->second;
      }
      if (auto it = meta_data.find("batch_size"); it != meta_data.end()) {
        config.batch_size = std::stoll(it->second);
      }
      return std::make_unique<ParquetWriter>(std::move(config));
    }

    const std::string& GetFilterName() const override { return ParquetWriter::kName; }
  };

}  // namespace cola_parquet_writer

#endif  // COLA_PARQUET_WRITER_PARQUETWRITERFACTORY_HH
