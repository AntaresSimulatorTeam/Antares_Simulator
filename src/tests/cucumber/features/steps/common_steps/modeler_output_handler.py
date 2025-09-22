# Antares modeler outputs parsing

import pandas as pd
import numpy as np

class modeler_output_handler:

    def __init__(self, simulation_table_location):
        self.simulation_table = modeler_output_handler.__read_simulation_table(simulation_table_location)

    @staticmethod
    def __read_simulation_table(absolute_path) -> pd.DataFrame:
        df = pd.read_csv(absolute_path, header=0, sep=",", low_memory=False)
        # We need an int or a range for scenario the modeler step 
        df['scenario_index'] = df['scenario_index'].replace(np.nan, 0)
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
