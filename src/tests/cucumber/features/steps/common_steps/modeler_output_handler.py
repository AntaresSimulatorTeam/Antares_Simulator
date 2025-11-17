# Antares modeler outputs parsing

import pandas as pd
import numpy as np
import common_steps.mps_utils as mpu
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
    def __read_simulation_table(absolute_path) -> pd.DataFrame:
        df = pd.read_csv(absolute_path, header=0, sep=",", low_memory=False)
        # We need an int or a range for scenario the modeler step 
        df['scenario_index'] = df['scenario_index'].replace("None", 0)
        df['scenario_index'] = df['scenario_index'].replace(np.nan, 0)
        df.replace("None", np.nan, inplace=True)
        cols = ["block", "absolute_time_index", "block_time_index", "scenario_index", "value"]
        for col in cols:
            df[col] = df[col].astype(float)
        return df

    def get_simulation_table_entry(self, component : str, output : str, block : int, timestep : int, scenario : int):
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
            raise LookupError(f"Simulation table contains {len(df)} row(s) (expected 1) for component '{component}', output '{output}', block '{block}', timestep '{timestep}', scenario '{scenario}'")
        return df["value"].iloc[0]

    def get_objective_value(self):
        df = self.simulation_table[(self.simulation_table["output"] == "OBJECTIVE_VALUE")]
        if len(df) != 1:
            raise LookupError(f"Simulation table contains no or multiple objective values")
        return df["value"].iloc[0]
