from behave import *
import subprocess
from pathlib import Path
import os

def run_simulation(context):
    command = build_antares_solver_command(context)
    print(f"Running command: {command}")
    process = subprocess.Popen(command, stdout=subprocess.PIPE, stderr=subprocess.DEVNULL)
    out, err = process.communicate()
    context.output_path = parse_output_folder(out)
    context.return_code = process.returncode
    context.annual_system_cost = None


def build_antares_solver_command(context):
    file = open('latest_binary_dir.txt', 'r')
    solver_path = os.path.join(file.readline(), "solver", "antares-solver")
    solver_full_path = str(Path(solver_path).resolve())
    command = [solver_full_path, "-i", str(context.study_path)]
    if context.use_ortools:
        command.append('--use-ortools')
        command.append('--ortools-solver=' + context.ortools_solver)
    if context.named_mps_problems:
        command.append('--named-mps-problems')
    if context.parallel:
        command.append('--force-parallel=4')
    return command


def parse_output_folder(logs : bytes) -> str:
    for line in logs.splitlines():
        if b'Output folder : ' in line:
            return line.split(b'Output folder : ')[1].decode('ascii')
    raise LookupError("Could not parse output folder in output logs")

def parse_annual_system_cost(output_path : str) -> dict:
    file = open(os.path.join(output_path, "annualSystemCost.txt"), 'r')
    keys = ["EXP", "MIN", "MAX"]
    annual_system_cost = {}
    for line in file.readlines():
        for key in keys:
            if key in line:
                annual_system_cost[key] = float(line.split(key + " : ")[1])
    return annual_system_cost