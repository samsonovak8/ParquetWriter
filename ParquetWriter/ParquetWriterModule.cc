#include "ParquetWriterFactory.hh"

#include <COLA.hh>

namespace {

  using ParquetWriterModule = cola::GenericModule<cola_parquet_writer::ParquetWriterFactory>;

}  // namespace

extern "C" cola::VModule* LoadCOLAModule() { return new ParquetWriterModule(); }
