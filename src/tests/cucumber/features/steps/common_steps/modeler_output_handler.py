# Antares modeler outputs parsing

import os
from pathlib import Path

from common_steps.simulation_table_checker import SimulationTable
from common_steps.simulation_table_reader import make_simulation_table_reader
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


def read_invest_problems(output_path: Path) -> invest_problems:
    """Read investment problem files (master.mps, 1-1.mps, structure.txt)."""
    output_location = str(output_path)
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


class modeler_output_handler:
    """Backward-compatible wrapper for solver tests.

    Solver tests still reference context.moh (modeler_output_handler),
    so we keep this class for compatibility. It delegates to SimulationTable.
    """

    def __init__(self, outputPath: Path, filePattern: str = "simulation-table*.csv"):
        self.simulation_table = SimulationTable(
            make_simulation_table_reader(outputPath, use_parquet=False)()
        )
        self.problems = read_invest_problems(outputPath)

    def get_objective_value(self):
        return self.simulation_table.get_objective_value()

    def get_objective_values_by_block(self):
        return self.simulation_table.get_objective_values_by_block()
