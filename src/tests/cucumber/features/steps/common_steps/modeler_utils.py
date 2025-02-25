# Methods to run Antares modeler

import subprocess
import os
from common_steps.modeler_output_handler import modeler_output_handler


def run_modeler(context):
    command = build_antares_modeler_command(context)
    print(f"Running command: {command}")
    process = subprocess.Popen(command, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    out, err = process.communicate()

    if process.returncode != 0:
        print("*********************** Begin stdout ***********************")
        print(out.replace(b'\r\n', b'\n').decode('utf-8'))
        print("*********************** End stdout ***********************")

        print("*********************** Begin stderr ***********************")
        print(err.replace(b'\r\n', b'\n').decode('utf-8'))
        print("*********************** End stderr ***********************")
    else:
        context.output_path = os.path.join(context.study_path,
                                           "output")  # TODO : fixme parse_output_folder_from_logs(out)
        context.moh = modeler_output_handler(context.output_path)

    context.return_code = process.returncode


def build_antares_modeler_command(context):
    command = [context.config.userdata["antares-modeler"], str(context.study_path)]
    return command


def parse_output_folder_from_logs(logs: bytes) -> str:
    for line in logs.splitlines():
        if b'Output folder : ' in line:
            return line.split(b'Output folder : ')[1].decode('ascii')
    raise LookupError("Could not parse output folder in output logs")
