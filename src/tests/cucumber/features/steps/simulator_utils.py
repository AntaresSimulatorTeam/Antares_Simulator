# Methods to run Antares simulator

import subprocess
import yaml
from pathlib import Path
from study_input_handler import study_input_handler
from output_utils import parse_output_folder_from_logs


def get_solver_path():
    with open("conf.yaml") as file:
        content = yaml.full_load(file)
    return content.get("antares-solver")


SOLVER_PATH = get_solver_path()  # we only need to run this once


def run_simulation(context):
    init_simu(context)
    init_empty_context(context)
    command = build_antares_solver_command(context)
    print(f"Running command: {command}")
    process = subprocess.Popen(command, stdout=subprocess.PIPE, stderr=subprocess.DEVNULL)
    out, err = process.communicate()
    context.output_path = parse_output_folder_from_logs(out)
    context.return_code = process.returncode

def init_simu(context):
    sih = study_input_handler(Path(context.study_path))
    # read metadata
    context.nbyears = int(sih.get_value(variable="nbyears", file_nick_name="general"))
    # activate year-by-year results  # TODO : remove this and update studies instead
    sih.set_value(variable="synthesis", value="true", file_nick_name="general")
    sih.set_value(variable="year-by-year", value="true", file_nick_name="general")

def init_empty_context(context):
    context.annual_system_cost = None
    context.values_annual = None
    context.values_hourly = None
    context.details_hourly = None


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
