# Antares modeler outputs parsing

import pandas as pd
import numpy as np
from shared_utils import mps_utils as mpu
import os


def read_if_exists(path, readfunc):
    if (os.path.exists(path)):
        return readfunc(path)
    else:
        return None


class invest_problems:
    def __init__(self, master, subproblem, structure):
        self.master = master
        self.subproblem = subproblem
        self.structure = structure


class modeler_output_handler:
    def __init__(self, simulation_table_location, output_location=None):
        self.simulation_table = modeler_output_handler.__read_simulation_table(simulation_table_location)
        if output_location:
            self.problems = modeler_output_handler.__read_problems(output_location)

    @staticmethod
    def __read_problems(output_location):
        # MASTER
        try:
            master = read_if_exists(os.path.join(output_location, "master.mps"), mpu.load_problem)
        except:
            master = None

        # SUBPROBLEM
        try:
            subproblem = read_if_exists(os.path.join(output_location, "1-1.mps"), mpu.load_problem)
        except:
            subproblem = None

        structure = read_if_exists(os.path.join(output_location, "structure.txt"), lambda x: open(x, 'r').readlines())
        return invest_problems(master, subproblem, structure)

    @staticmethod
    def __read_simulation_table(simulation_table_location) -> pd.DataFrame:
        if isinstance(simulation_table_location, (list, tuple)):
            paths = list(simulation_table_location)
        else:
            paths = [simulation_table_location]
        frames = [pd.read_csv(p, header=0, sep=",", low_memory=False) for p in paths]
        df = pd.concat(frames, ignore_index=True) if len(frames) > 1 else frames[0]
        # We need an int or a range for scenario the modeler step
        df['scenario_index'] = df['scenario_index'].replace("None", 0)
        df['scenario_index'] = df['scenario_index'].replace(np.nan, 0)
        df.replace("None", np.nan, inplace=True)
        cols = ["block", "absolute_time_index", "block_time_index", "scenario_index", "value"]
        for col in cols:
            df[col] = df[col].astype(float)
        return df

    @staticmethod
    def __build_lookup_error_msg(component, output, block, timestep, scenario, df, simulation_table):
        """Build a detailed error message for simulation table lookup failures."""
        all_components = simulation_table["component"].dropna().unique().tolist()
        all_outputs = simulation_table["output"].dropna().unique().tolist()
        available_components = sorted([str(c) for c in all_components])
        available_outputs = sorted([str(o) for o in all_outputs])

        df_for_comp = simulation_table[(simulation_table["component"] == component)]
        available_for_comp = sorted(df_for_comp["absolute_time_index"].unique().tolist()) if not df_for_comp.empty else "none"

        df_comp_out = simulation_table[(simulation_table["component"] == component)
                                       & (simulation_table["output"] == output)]
        available_timesteps = sorted(df_comp_out["absolute_time_index"].unique().tolist()) if not df_comp_out.empty else "n/a (no component/output match)"

        return (
            f"Simulation table lookup failed for:\n"
            f"  component: '{component}'\n"
            f"  output: '{output}'\n"
            f"  block: '{block}'\n"
            f"  timestep: '{timestep}'\n"
            f"  scenario: '{scenario}'\n"
            f"Found {len(df)} row(s) (expected 1).\n"
            f"Available components: {available_components}\n"
            f"Available outputs: {available_outputs}\n"
            f"Available timesteps for component '{component}': {available_for_comp}\n"
            f"Available absolute_time_index values: {available_timesteps}")

    def get_simulation_table_entry(self, component: str, output: str, block: int, timestep: int, scenario: int):
        df = self.simulation_table[(self.simulation_table["component"] == component)
                                   & (self.simulation_table["output"] == output)]
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
            raise LookupError(self.__build_lookup_error_msg(component, output, block, timestep, scenario, df, self.simulation_table))
        return df["value"].iloc[0]

    def get_objective_value(self):
        df = self.simulation_table[(self.simulation_table["output"] == "OBJECTIVE_VALUE")][self.simulation_table["scenario_index"] == 0]
        if len(df) != 1:
            raise LookupError(f"Simulation table contains no or multiple objective values")
        return df["value"].iloc[0]

    def get_objective_values_by_block(self):
        """
        Returns a dictionary mapping block number to objective value.
        Each block represents a time step in the optimization.
        """
        df = self.simulation_table[(self.simulation_table["output"] == "OBJECTIVE_VALUE")]
        if df.empty:
            raise LookupError(f"Simulation table contains no objective values")
        # Group by block and get the objective value for each block
        result = {}
        for _, row in df.iterrows():
            block = int(row["block"])
            value = row["value"]
            result[block] = value
        return result
