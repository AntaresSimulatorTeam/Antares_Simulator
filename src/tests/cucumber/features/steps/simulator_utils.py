# Methods to run Antares simulator

import subprocess
import os
from pathlib import Path
from study_input_handler import study_input_handler
from output_utils import parse_output_folder_from_logs


def get_solver_path():
    file = open('latest_binary_dir.txt', 'r')
    if os.name == 'nt':
        exec_name = "antares-solver.exe"
    else:
        exec_name = "antares-solver"
    solver_path = os.path.join(file.readline(), "solver", exec_name)
    solver_full_path = str(Path(solver_path).resolve())
    return solver_full_path

SOLVER_PATH = get_solver_path() # we only need to run this once


def run_simulation(context):
    activate_simu_outputs(context)  # TODO : remove this and update studies instead
    command = build_antares_solver_command(context)
    print(f"Running command: {command}")
    process = subprocess.Popen(command, stdout=subprocess.PIPE, stderr=subprocess.DEVNULL)
    out, err = process.communicate()
    context.output_path = parse_output_folder_from_logs(out)
    context.return_code = process.returncode
    context.annual_system_cost = None
    context.hourly_values = None


def activate_simu_outputs(context):
    sih = study_input_handler(Path(context.study_path))
    sih.set_value(variable="synthesis", value="true", file_nick_name="general")
    sih.set_value(variable="year-by-year", value="true", file_nick_name="general")


def build_antares_solver_command(context):
    command = [SOLVER_PATH, "-i", str(context.study_path)]
    if context.use_ortools:
        command.append('--use-ortools')
        command.append('--ortools-solver=' + context.ortools_solver)
    if context.named_mps_problems:
        command.append('--named-mps-problems')
    if context.parallel:
        command.append('--force-parallel=4')
    return command

