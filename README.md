# ParquetWriter
`ParquetWriter` is a configurable `cola::VWriter` implementation that serializes `cola::EventData` events and writes them to disk as Apache Parquet files.

## Dependencies
- An installed COLA core (`find_package(COLA)`), `~/.local` by default
- `libarrow-dev`, `libparquet-dev`
- Python: `colapy`, `pyarrow`, `pytest`

## Build and install
```bash
cmake -S . -B build \
    -DCMAKE_PREFIX_PATH="$HOME/.local" \
    -DCMAKE_INSTALL_PREFIX="$HOME/.local"
cmake --build build -j
cmake --install build
```

## Configuration (XML attributes)
| Attribute     | Default               | Description                              |
|---------------|-----------------------|------------------------------------------|
| `path`        | `cola_output.parquet` | Output file path                         |
| `compression` | `zstd`                | `zstd` / `snappy` / `gzip` / `none`      |
| `batch_size`  | `1000`                | Number of events per Parquet row group   |

Use in COLA pipeline:
```xml
<writer name="ParquetWriter" path="out.parquet" compression="zstd" batch_size="64"/>