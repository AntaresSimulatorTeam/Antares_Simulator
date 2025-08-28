# Antares modeler outputs parsing

import pandas as pd


class modeler_output_handler:

    def __init__(self, simulation_table_location):
        self.simulation_table = self.__read_csv(simulation_table_location)

    def __read_csv(self, absolute_path) -> pd.DataFrame:
        return pd.read_csv(absolute_path, header=0, sep=',', low_memory=False)

    def get_simulation_table_entry(self, component : str, output : str, timestep : int, scenario : int):
        df = self.simulation_table[(self.simulation_table["component"] == component)
                                   & (self.simulation_table["output"] == output)
                                   & (self.simulation_table["absolute_time_index"] == timestep)
                                   & (self.simulation_table["scenario_index"] == scenario)]
        if len(df) != 1:
            raise LookupError(f"Simulation table does not contain exactly 1 row for component '{component}', output '{output}', timestep '{timestep}', scenario '{scenario}'")
        return df["value"].iloc[0]

    def get_objective_value(self):
        df = self.simulation_table[(self.simulation_table["output"] == "OBJECTIVE_VALUE")]
        if len(df) != 1:
            raise LookupError(f"Simulation table contains no or multiple objective values")
        return df["value"].iloc[0]
