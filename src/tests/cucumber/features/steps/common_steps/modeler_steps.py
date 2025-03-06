# Test steps definitions specific to antares-modeler

import os
import subprocess

from behave import *
from common_steps.assertions import *
from common_steps.modeler_output_handler import modeler_output_handler


@given('the modeler study path is "{string}"')
def modeler_study_path_is(context, string):
    context.study_path = os.path.join(context.config.userdata["resources-path"], string.replace("/", os.sep))

@when("I run antares modeler")
def run_antares_modeler(context):
    run_modeler(context)


@step('the optimal value of variable {var} is {value:g}')
def modeler_var_optimal_value(context, var, value):
    assert_double_close(value, context.moh.get_optimal_value(var), 1e-6)


@step('the objective value is {value:g}')
def modeler_obj_value(context, value):
    assert_double_close(value, context.moh.get_optimal_value("objective"), 1e-6)

@step('the optimal values of the variables are')
def modeler_var_optimal_value(context):
    for row in context.table:
        ts_array = row["timestep"].split("-")
        ts_start = int(ts_array[0])
        ts_end =  int(ts_array[1]) if len(ts_array) == 2 else ts_start
        for ts in range(ts_start, ts_end + 1):
            var_id = row["component"] + "." + row["variable"] + "_t" + str(ts)
            assert_double_close(get_value(row, ts), context.moh.get_optimal_value(var_id), 1e-6)


def get_value(row, ts):
    ret = row["value"]

    if "-" in ret and not ret.isdigit():  # Handle "80-0" but not single numbers
        ret = ret.split("-")  # Split into a list of strings
        return float(ret[ts])  # Index and convert to float

    return float(ret)  # Single value case (apply to all timesteps)


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