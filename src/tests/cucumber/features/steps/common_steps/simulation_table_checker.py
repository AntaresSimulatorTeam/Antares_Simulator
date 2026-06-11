# Simulation table class and checkers (business logic)
# The SimulationTable class wraps a pandas DataFrame and provides
# query methods. These are format-agnostic: the same methods work
# for CSV or Parquet data.

from typing import Optional

import numpy as np
import pandas as pd


def _build_lookup_error_msg(
    component: str,
    output: str,
    block: Optional[float],
    timestep: Optional[float],
    scenario: Optional[float],
    df: pd.DataFrame,
    simulation_table: pd.DataFrame,
) -> str:
    """Build a detailed error message for simulation table lookup failures."""
    all_components = sorted(
        simulation_table["component"].dropna().unique().tolist()
    )
    all_outputs = sorted(
        simulation_table["output"].dropna().unique().tolist()
    )

    df_for_comp = simulation_table[simulation_table["component"] == component]
    available_for_comp = (
        sorted(df_for_comp["absolute_time_index"].unique().tolist())
        if not df_for_comp.empty
        else "none"
    )

    df_comp_out = simulation_table[
        (simulation_table["component"] == component)
        & (simulation_table["output"] == output)
    ]
    available_timesteps = (
        sorted(df_comp_out["absolute_time_index"].unique().tolist())
        if not df_comp_out.empty
        else "n/a (no component/output match)"
    )

    return (
        f"Simulation table lookup failed for:\n"
        f"  component: '{component}'\n"
        f"  output: '{output}'\n"
        f"  block: '{block}'\n"
        f"  timestep: '{timestep}'\n"
        f"  scenario: '{scenario}'\n"
        f"Found {len(df)} row(s) (expected 1).\n"
        f"Available components: {all_components}\n"
        f"Available outputs: {all_outputs}\n"
        f"Available timesteps for component '{component}': {available_for_comp}\n"
        f"Available absolute_time_index values: {available_timesteps}"
    )


class SimulationTable:
    """Wraps a pandas DataFrame representing a simulation table.

    Provides query methods for looking up entries, objective values,
    and column information.
    """

    def __init__(self, dataframe: pd.DataFrame):
        self._dataframe = dataframe

    def get_entry(
        self,
        component: str,
        output: str,
        block: Optional[float],
        timestep: Optional[float],
        scenario: Optional[float],
    ) -> float:
        """Look up a single entry by component, output, block, timestep, and scenario."""
        df = self._dataframe[
            (self._dataframe["component"] == component)
            & (self._dataframe["output"] == output)
        ]
        if not pd.isna(block):
            df = df[df["block"] == block]
        if pd.isna(timestep):
            df = df[pd.isna(df["absolute_time_index"])]
        else:
            df = df[df["absolute_time_index"] == timestep]
        if pd.isna(scenario):
            df = df[pd.isna(df["scenario_index"])]
        else:
            df = df[df["scenario_index"] == scenario]

        if len(df) != 1:
            raise LookupError(
                _build_lookup_error_msg(component, output, block, timestep, scenario, df, self._dataframe)
            )
        return float(df["value"].iloc[0])

    def get_objective_value(self) -> float:
        """Returns the objective value from the simulation table (scenario_index == 0)."""
        df = self._dataframe[
            (self._dataframe["output"] == "OBJECTIVE_VALUE")
            & (self._dataframe["scenario_index"] == 0)
        ]
        if len(df) != 1:
            raise LookupError(
                f"Simulation table contains {len(df)} objective value(s), expected 1"
            )
        value = df["value"].iloc[0]
        if pd.isna(value):
            raise LookupError("Simulation table objective value is NULL")
        return float(value)

    def get_objective_values_by_block(self) -> dict:
        """Returns a dictionary mapping block number to objective value."""
        df = self._dataframe[self._dataframe["output"] == "OBJECTIVE_VALUE"]
        if df.empty:
            raise LookupError("Simulation table contains no objective values")

        result = {}
        for _, row in df.iterrows():
            block = int(row["block"])
            value = row["value"]
            result[block] = float(value)
        return result

    def get_dataframe(self) -> pd.DataFrame:
        """Return the full DataFrame for advanced queries."""
        return self._dataframe

    def get_column_names(self) -> list:
        """Return the list of column names in the simulation table."""
        return list(self._dataframe.columns)


def get_simulation_table_dataframe(simulation_table: pd.DataFrame) -> pd.DataFrame:
    """Return the full DataFrame for advanced queries."""
    return simulation_table


def get_column_names(simulation_table: pd.DataFrame) -> list:
    """Return the list of column names in the simulation table."""
    return list(simulation_table.columns)
