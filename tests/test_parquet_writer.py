import os
import sys

import colapy
import pyarrow.parquet as pq
import pytest

sys.path.insert(0, os.path.dirname(__file__))


def pipeline_config_xml(
    path: str,
    min_p: int,
    max_p: int,
    seed: int,
    batch_size: int,
    compression: str = "zstd",
) -> str:
    return f"""<?xml version="1.0" encoding="UTF-8"?>
<program>
    <generator name="PythonGenerator" class="variable_generator.VariableGenerator"
               min_particles="{min_p}" max_particles="{max_p}" seed="{seed}"/>
    <writer name="ParquetWriter"
            path="{path}" compression="{compression}" batch_size="{batch_size}"/>
</program>
"""


def run_pipeline(config_xml: str, steps: int) -> None:
    manager = colapy.RunManager()
    manager.load_module("COLA-Py")
    manager.load_module("ParquetWriter")
    manager.load_config(config=config_xml)
    manager.run(steps)
    del manager


@pytest.mark.parametrize(
    "min_p, max_p, seed, batch_size, compression, num_events, expect_variable",
    [
        pytest.param(0, 50, 42, 64, "zstd", 200, True, id="variable-zstd"),
        pytest.param(0, 0, 1, 4, "none", 10, False, id="empty-none"),
    ],
)
def test_cpp_writer(
    tmp_path, min_p, max_p, seed, batch_size, compression, num_events, expect_variable
):
    out = str(tmp_path / "cola_test.parquet")
    run_pipeline(
        pipeline_config_xml(out, min_p, max_p, seed, batch_size, compression),
        num_events,
    )
    table = pq.read_table(out)
    assert table.num_rows == num_events
    assert {"particles", "ini_state_particles"} <= set(table.schema.names)
    lengths = {len(x) for x in table.column("particles").to_pylist()}
    if expect_variable:
        assert len(lengths) > 1, "particles number might change"
    else:
        assert lengths == {0}
