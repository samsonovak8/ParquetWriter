# ParquetWriter
`ParquetWriter` is a configurable `cola::VWriter` implementation that serializes `cola::EventData` events and writes them to disk as Apache Parquet files.

## Dependencies
- An installed COLA core (`find_package(COLA)`), `~/.local` by default
- `libarrow-dev`, `libparquet-dev`
- Python: `colapy`, `pyarrow`, `pytest`

## Build and install
```bash
source ~/.venv-cola/bin/activate
cmake -S . -B build -DCMAKE_PREFIX_PATH="$HOME/.local" -DBUILD_TESTING=ON
cmake --build build -j && cmake --install build
ctest --test-dir build --output-on-failure # for tests
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