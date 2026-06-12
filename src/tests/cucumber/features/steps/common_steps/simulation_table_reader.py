# Simulation table readers (CSV / Parquet)
# These readers decouple format-specific parsing from business logic checks.
# They return pandas DataFrames that can be consumed by shared check methods.

from enum import Enum
from io import StringIO
from pathlib import Path

import numpy as np
import pandas as pd

import pyarrow as arrow
import pyarrow.parquet as parquet


class OutputFormat(Enum):
    """Output file format for simulation tables."""

    CSV = "csv"
    PARQUET = "parquet"


def accumulate_simu_table_files(directory: Path, filePattern: str) -> str:
    """
    Accumulate contents of all simulation-table*.csv files into a single string.
    Keeps the CSV header from the first file and removes it from subsequent files.
    """
    accumulated = []
    for i, csv_file in enumerate(sorted(directory.glob(filePattern))):
        content = csv_file.read_text(encoding='utf-8')

        if i == 0:
            accumulated.append(content)
        else:
            lines = content.split('\n', 1)
            if len(lines) > 1:
                accumulated.append(lines[1])
    return "".join(accumulated)


def read_simulation_table_csv(outputPath: Path, filePattern: str) -> pd.DataFrame:
    """
    Read simulation-table*.csv files from the output directory.
    If multiple files exist (e.g., for multiple optimisations), concatenate them.
    Returns a pandas DataFrame.
    """
    simu_table_str = accumulate_simu_table_files(outputPath, filePattern)
    if not simu_table_str:
        raise FileNotFoundError(
            f"No simulation table files matching '{filePattern}' found in '{outputPath}'"
        )
    df = pd.read_csv(StringIO(simu_table_str), header=0, sep=",", low_memory=False)
    return _normalize_simulation_table(df)


def read_simulation_table_parquet(outputPath: Path, filePattern: str) -> pd.DataFrame:
    """
    Read simulation-table*.parquet files from the output directory.
    If multiple files exist (e.g., for multiple optimisations), concatenate them.
    Returns a pandas DataFrame.
    """

    files = sorted(outputPath.glob(filePattern))
    if not files:
        raise FileNotFoundError(
            f"No simulation table files matching '{filePattern}' found in '{outputPath}'"
        )

    tables = [parquet.read_table(f) for f in files]
    pa_table = arrow.concat_tables(tables)

    df = pa_table.to_pandas()
    return _normalize_simulation_table(df)


def _normalize_simulation_table(df: pd.DataFrame) -> pd.DataFrame:
    """
    Normalize a simulation table DataFrame:
    - Replace 'None' strings and NaN in scenario_index with 0
    - Convert numeric columns to float
    """
    df = df.copy()
    df['scenario_index'] = df['scenario_index'].replace("None", 0)
    df['scenario_index'] = df['scenario_index'].replace(np.nan, 0)
    df.replace("None", np.nan, inplace=True)
    cols = ["block", "absolute_time_index", "block_time_index", "scenario_index", "value"]
    for col in cols:
        if col in df.columns:
            df[col] = df[col].astype(float)
    return df


def make_simulation_table_reader(
    output_path: Path,
    output_format: OutputFormat,
    file_pattern: str | None = None,
) -> object:
    """Factory function that returns the appropriate simulation table reader.

    Args:
        output_path: Path to the output directory containing simulation table files.
        output_format: CSV or Parquet format.
        file_pattern: Glob pattern for simulation table files.
            Defaults to the solver pattern "simulation-table-*-optim-nb-1.{format}".
            For modeler output, use "simulation-table*.{format}".
    """
    if file_pattern is None:
        file_pattern = f"simulation-table-*-optim-nb-1.{output_format.value}"
    if output_format == OutputFormat.PARQUET:
        return lambda: read_simulation_table_parquet(output_path, file_pattern)
    else:
        return lambda: read_simulation_table_csv(output_path, file_pattern)


def make_modeler_simulation_table_reader(
    output_path: Path, output_format: OutputFormat
) -> object:
    """Factory function for antares-modeler simulation table output.

    The modeler writes files named "simulation-table.{format}" (no year/optim suffix),
    so the glob pattern is "simulation-table*.{format}".
    """
    file_pattern = f"simulation-table*.{output_format.value}"
    if output_format == OutputFormat.PARQUET:
        return lambda: read_simulation_table_parquet(output_path, file_pattern)
    else:
        return lambda: read_simulation_table_csv(output_path, file_pattern)
