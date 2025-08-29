# Antares modeler outputs parsing

import pandas as pd


class modeler_output_handler:

    def __init__(self, simulation_table_location):
        self.simulation_table = self.__read_csv(simulation_table_location)

    def __read_csv(self, absolute_path) -> pd.DataFrame:
        return pd.read_csv(absolute_path, header=0, sep=',', low_memory=False)

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
            raise LookupError(f"Simulation table does not contain exactly 1 row for component '{component}', output '{output}', block '{block}', timestep '{timestep}', scenario '{scenario}'")
        return df["value"].iloc[0]

    def get_objective_value(self):
        df = self.simulation_table[(self.simulation_table["output"] == "OBJECTIVE_VALUE")]
        if len(df) != 1:
            raise LookupError(f"Simulation table contains no or multiple objective values")
        return df["value"].iloc[0]
