# Antares modeler outputs parsing

import os
from pathlib import Path

from common_steps.simulation_table_checker import (
    get_column_names,
    get_objective_value,
    get_objective_values_by_block,
    get_simulation_table_dataframe,
    get_simulation_table_entry,
)
from common_steps.simulation_table_reader import read_simulation_table_csv
from shared_utils import mps_utils as mpu


def read_if_exists(path, readfunc):
    if os.path.exists(path):
        return readfunc(path)
    else:
        return None


class invest_problems:
    def __init__(self, master, subproblem, structure):
        self.master = master
        self.subproblem = subproblem
        self.structure = structure


class modeler_output_handler:
    def __init__(self, outputPath: Path, filePattern: str, readInvestFiles=False):
        self.simulation_table = read_simulation_table_csv(outputPath, filePattern)
        if readInvestFiles:
            self.problems = self.__read_problems(outputPath)

    @staticmethod
    def __read_problems(outputPath):
        output_location = str(outputPath)
        try:
            master = read_if_exists(os.path.join(output_location, "master.mps"), mpu.load_problem)
        except Exception:
            master = None

        try:
            subproblem = read_if_exists(os.path.join(output_location, "1-1.mps"), mpu.load_problem)
        except Exception:
            subproblem = None

        structure = read_if_exists(
            os.path.join(output_location, "structure.txt"),
            lambda x: open(x, 'r').readlines(),
        )
        return invest_problems(master, subproblem, structure)

    def get_simulation_table_entry(self, component: str, output: str, block, timestep, scenario):
        return get_simulation_table_entry(self.simulation_table, component, output, block, timestep, scenario)

    def get_objective_value(self):
        return get_objective_value(self.simulation_table)

    def get_objective_values_by_block(self):
        return get_objective_values_by_block(self.simulation_table)

    def get_table(self):
        return get_simulation_table_dataframe(self.simulation_table)

    def get_column_names(self):
        return get_column_names(self.simulation_table)
