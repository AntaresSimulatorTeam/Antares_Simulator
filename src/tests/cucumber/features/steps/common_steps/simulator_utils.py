# Methods to run Antares simulator

import subprocess
from pathlib import Path
from common_steps.study_input_handler import study_input_handler
from common_steps.study_output_handler import study_output_handler


def run_simulation(context):
    init_simu(context)
    command = build_antares_solver_command(context)
    print(f"Running command: {command}")
    process = subprocess.Popen(command, stdout=subprocess.PIPE, stderr=subprocess.DEVNULL)
    out, err = process.communicate()
    context.output_path = parse_output_folder_from_logs(out)
    context.return_code = process.returncode
    context.soh = study_output_handler(context.output_path)


def init_simu(context):
    sih = study_input_handler(Path(context.study_path))
    # read metadata
    context.nbyears = int(sih.get_value(variable="nbyears", file_nick_name="general"))
    # activate year-by-year results  # TODO : remove this and update studies instead
    sih.set_value(variable="synthesis", value="true", file_nick_name="general")
    sih.set_value(variable="year-by-year", value="true", file_nick_name="general")


def build_antares_solver_command(context):
    command = [context.config.userdata["antares-solver"], "-i", str(context.study_path)]
    if "use-ortools" in context.config.userdata and context.config.userdata["use-ortools"].lower() == 'true':
        command.append('--use-ortools')
        if "ortools-solver" in context.config.userdata:
            solver = context.config.userdata["ortools-solver"]
        else:
            solver = "sirius"
        command.append('--ortools-solver=' + solver)
    if context.named_mps_problems:
        command.append('--named-mps-problems')
    if context.parallel:
        command.append('--force-parallel=4')
    return command


def parse_output_folder_from_logs(logs: bytes) -> str:
    for line in logs.splitlines():
        if b'Output folder : ' in line:
            return line.split(b'Output folder : ')[1].decode('ascii')
    raise LookupError("Could not parse output folder in output logs")
