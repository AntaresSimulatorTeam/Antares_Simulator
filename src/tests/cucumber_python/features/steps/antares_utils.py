import subprocess
from pathlib import Path
import os
import time
import datetime
from study_input_handler import study_input_handler
from antares_csv_utils import read_csv

def run_simulation(context):
    activate_simu_outputs(context) # TODO : remove this and update studies instead
    command = build_antares_solver_command(context)
    print(f"Running command: {command}")
    process = subprocess.Popen(command, stdout=subprocess.PIPE, stderr=subprocess.DEVNULL)
    out, err = process.communicate()
    context.output_path = parse_output_folder(out)
    context.return_code = process.returncode
    context.annual_system_cost = None
    context.hourly_values = None

def activate_simu_outputs(context):
    sih = study_input_handler(Path(context.study_path))
    sih.set_value(variable="synthesis", value="true", file_nick_name="general")
    sih.set_value(variable="year-by-year", value="true", file_nick_name="general")

def build_antares_solver_command(context):
    file = open('latest_binary_dir.txt', 'r')
    if os.name() == 'nt':
        exec_name = "antares-solver.exe"
    else:
        exec_name = "antares-solver"
    solver_path = os.path.join(file.readline(), "solver", exec_name)
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
    keys = ["EXP", "STD", "MIN", "MAX"]
    annual_system_cost = {}
    for line in file.readlines():
        for key in keys:
            if key in line:
                annual_system_cost[key] = float(line.split(key + " : ")[1])
    return annual_system_cost

def parse_simu_time(output_path: str) -> float:
    file = open(os.path.join(output_path, "simulation.log"), 'r')
    for line in file.readlines():
        if "Total simulation time:" in line:
            x = time.strptime(line[-10 : -1],'%Hh%Mm%Ss')
            return datetime.timedelta(hours=x.tm_hour,minutes=x.tm_min,seconds=x.tm_sec).total_seconds()

def parse_hourly_values(output_path: str, area : str, year : int):
    return read_csv(os.path.join(output_path, "economy", "mc-ind", f"{year:05d}", "areas", area, "values-hourly.txt"))