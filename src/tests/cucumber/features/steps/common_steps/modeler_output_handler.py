# Antares modeler outputs parsing

import pandas as pd
import numpy as np
from shared_utils import mps_utils as mpu
import os
from io import StringIO
from pathlib import Path

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

def accumulate_simu_table_files(directory: Path, filePattern: str) -> str:
    """
    Accumulate contents of all simulation-table*.csv files into a single string.
    Keeps the CSV header from the first file and removes it from subsequent files.
    Returns a valid CSV header if no files are found (for executables that don't
    produce a simulation table, e.g., antares-problem-generator).
    """
    files = sorted(directory.glob(filePattern))
    if not files:
        return "block,component,output,absolute_time_index,block_time_index,scenario_index,value,basis_status\n"

    accumulated = []
    for i, csv_file in enumerate(files):
        content = csv_file.read_text(encoding='utf-8')

        if i == 0:
            # Keep the entire first file including header
            accumulated.append(content)
        else:
            # Skip the "block, component,output,absolute_time_index,..." header line for subsequent files
            lines = content.split('\n', 1)
            if len(lines) > 1:
                # Add only the data part (skip first line which is the header)
                accumulated.append(lines[1])
            # If file only has header or is empty, skip it
    return "".join(accumulated)

class modeler_output_handler:
    def __init__(self, outputPath: Path, filePattern: str, readInvestFiles=False):
        self.simulation_table = modeler_output_handler.__read_simulation_table(outputPath, filePattern)
        if readInvestFiles:
            self.problems = modeler_output_handler.__read_problems(outputPath)

    @staticmethod
    def __read_problems(outputPath):
        output_location = str(outputPath)
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
    def __read_simulation_table(outputPath: Path, filePattern: str) -> pd.DataFrame:
        simu_table_str = accumulate_simu_table_files(outputPath, filePattern)
        df = pd.read_csv(StringIO(simu_table_str), header=0, sep=",", low_memory=False)

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
