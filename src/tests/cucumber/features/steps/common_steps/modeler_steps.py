#  Copyright 2007-2025, RTE (https://www.rte-france.com)
#  See AUTHORS.txt
#  SPDX-License-Identifier: MPL-2.0
#  This file is part of Antares-Simulator,
#  Adequacy and Performance assessment for interconnected energy networks.
#
#  Antares_Simulator is free software: you can redistribute it and/or modify
#  it under the terms of the Mozilla Public Licence 2.0 as published by
#  the Mozilla Foundation, either version 2 of the License, or
#  (at your option) any later version.
#
#  Antares_Simulator is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
#  Mozilla Public Licence 2.0 for more details.
#
#  You should have received a copy of the Mozilla Public Licence 2.0
#  along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.

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


@step('the objective value is greater than {lb:g} and lower than {ub:g}')
def modeler_obj_value(context, lb, ub):
    assert lb <= context.moh.get_optimal_value(
        "objective") <= ub, f"Objective value is not inside expected range: {context.moh.get_optimal_value('objective')}"


@step('the optimal values of the variables are')
def modeler_var_optimal_value(context):
    for row in context.table:
        print("Processing row:", row)
        ts_array = row["timestep"].split("-")
        ts_start = int(ts_array[0])
        ts_end = int(ts_array[1]) if len(ts_array) == 2 else ts_start
        if "scenarioInterval" not in context.table.headings:
            print("No scenarioInterval specified, using default values")
            scenarioInterval_start = 0
            scenarioInterval_end = 0
        else:
            scenarioInterval_array = row["scenarioInterval"].split("-")
            scenarioInterval_start = int(scenarioInterval_array[0])
            scenarioInterval_end = int(scenarioInterval_array[1]) if len(
                scenarioInterval_array) == 2 else scenarioInterval_start
        for scenarioInterval in range(scenarioInterval_start, scenarioInterval_end + 1):
            for ts in range(ts_start, ts_end + 1):
                var_id = row["component"] + "." + row["variable"] + "_s" + str(scenarioInterval) + "_t" + str(ts)
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
    if out:
        context.logs_out = out.decode("utf-8")
    else:
        context.logs_out = ""
    if err:
        context.logs_err = err.decode("utf-8")
    else:
        context.logs_err = ""

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
