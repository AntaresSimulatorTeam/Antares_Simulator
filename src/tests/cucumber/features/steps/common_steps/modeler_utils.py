# Methods to run Antares modeler

import subprocess
import os



def run_modeler(context):
    command = build_antares_modeler_command(context)
    print(f"Running command: {command}")
    process = subprocess.Popen(command, stdout=subprocess.PIPE, stderr=subprocess.DEVNULL)
    out, err = process.communicate()
    context.return_code = process.returncode


def build_antares_modeler_command(context):
    command = [context.config.userdata["antares-modeler"], str(context.study_path)]
    return command


def parse_output_folder_from_logs(logs: bytes) -> str:
    for line in logs.splitlines():
        if b'Output folder : ' in line:
            return line.split(b'Output folder : ')[1].decode('ascii')
    raise LookupError("Could not parse output folder in output logs")
