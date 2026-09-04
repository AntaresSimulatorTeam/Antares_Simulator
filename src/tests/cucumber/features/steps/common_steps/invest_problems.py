# Antares modeler outputs parsing

import os
from pathlib import Path

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
    """Read investment problem files (master.mps, 0-1.mps, structure.txt)."""
    output_location = str(output_path)
    try:
        master = read_if_exists(os.path.join(output_location, "master.mps"), mpu.load_problem)
    except Exception:
        master = None

    try:
        subproblem = read_if_exists(os.path.join(output_location, "0-1.mps"), mpu.load_problem)
    except Exception:
        subproblem = None

    structure = read_if_exists(
        os.path.join(output_location, "structure.txt"),
        lambda x: open(x, 'r').readlines(),
    )
    return invest_problems(master, subproblem, structure)
