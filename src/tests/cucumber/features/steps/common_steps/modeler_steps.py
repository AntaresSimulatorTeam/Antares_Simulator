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
import math
# Test steps definitions specific to antares-modeler

import os
import subprocess

from behave import *
from common_steps.assertions import *
from common_steps.modeler_output_handler import modeler_output_handler
import common_steps.mps_utils as mpu


@given('the modeler study path is "{string}"')
def modeler_study_path_is(context, string):
    context.study_path = os.path.join(context.config.userdata["resources-path"], string.replace("/", os.sep))


@when("I run antares modeler")
def run_antares_modeler(context):
    run_modeler(context)


@step('the objective value is {value:g}')
def modeler_obj_value(context, value):
    assert_double_close(value, context.moh.get_objective_value(), 1e-6)


@step('the objective value is greater than {lb:g} and lower than {ub:g}')
def modeler_obj_value(context, lb, ub):
    assert lb <= context.moh.get_objective_value() <= ub, \
        f"Objective value is not inside expected range: {context.moh.get_objective_value()}"


@step('the modeler outputs contain the following entries')
def modeler_output_values(context):
    for row in context.table:
        ts_range = read_int_range(row, "timestep")
        if "scenario" not in context.table.headings:
            scenario_range = [0]
        else:
            scenario_range = read_int_range(row, "scenario")
        if "block" in context.table.headings:
            block_range = read_int_range(row, "block")
        else:
            block_range = [math.nan]
        for block in block_range:
            for scenario in scenario_range:
                for ts in ts_range:
                    assert_double_close(
                        get_value(row, ts), context.moh.get_simulation_table_entry(row["component"], row["output"], block, ts, scenario), 1e-6
                    )

def read_int_range(row, key : str):
    if row[key] != "":
        array = row[key].split("-")
        start = int(array[0])
        end = int(array[1]) if len(array) == 2 else start
        return range(start, end + 1)
    else:
        return [math.nan]

def get_value(row, ts):
    ret = row["value"]

    # if "-" in ret and not ret.isdigit():  # Handle "80-0" but not single numbers
    #     ret = ret.split("-")  # Split into a list of strings
    #     return float(ret[ts])  # Index and convert to float

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
        context.moh = modeler_output_handler(parse_simulation_table_from_logs(context.logs_out), parse_output_folder_from_logs(context.logs_out))

    context.return_code = process.returncode


def parse_simulation_table_from_logs(logs: str) -> str:
    for line in logs.splitlines():
        if 'Simulation table is written in: ' in line:
            return line.split('Simulation table is written in: ')[1]
    raise LookupError("Could not find simulation table location in output logs")



def build_antares_modeler_command(context):
    command = [context.config.userdata["antares-modeler"], str(context.study_path)]
    return command


def parse_output_folder_from_logs(logs: str) -> str:
    for line in logs.splitlines():
        if 'Output folder : ' in line:
            return line.split('Output folder : ')[1]
    raise LookupError("Could not parse output folder in output logs")


def check_variables(context, model):
    actual = mpu.extract_variables(model)

    # context.table is the Gherkin table passed in the feature
    expected = [row.as_dict() for row in context.table]

    # convert types
    for row in expected:
        row['xmin'] = None if row['xmin'] in ("None", "") else float(row['xmin'])
        row['xmax'] = None if row['xmax'] in ("None", "") else float(row['xmax'])
        row['cost'] = float(row['cost'])

    # simple comparison
    for e in expected:
        match = next((a for a in actual if a['name'] == e['name']), None)
        assert match is not None, f"Variable {e['name']} not found in model"
        assert match['xmin'] == e['xmin'], f"{e['name']} xmin mismatch: {match['xmin']} != {e['xmin']}"
        assert match['xmax'] == e['xmax'], f"{e['name']} xmax mismatch: {match['xmax']} != {e['xmax']}"
        assert match['cost'] == e['cost'], f"{e['name']} cost mismatch: {match['cost']} != {e['cost']}"


@then(u'the master problem contains the following variables')
def check_master_variables(context):
    assert(context.moh.problems != None and context.moh.problems.master != None)
    check_variables(context, context.moh.problems.master)

@then(u'the subproblem contains the following variables')
def check_subproblem_variables(context):
    assert(context.moh.problems != None and context.moh.problems.subproblem != None)
    model = context.moh.problems.subproblem
    check_variables(context, context.moh.problems.subproblem)


def parse_structure(content):
    entries = []
    for line in content:
        line = line.strip()
        if not line:
            continue  # skip blank lines
        parts = line.split()
        if len(parts) != 3:
            raise ValueError(f"Invalid structure entry: {line}")
        problem, variable, index = parts
        entries.append({
            "problem": problem,
            "variable": variable,
            "index": int(index),
        })
    return entries

@then(u'the structure file contains the following entries')
def check_structure(context):
    structure = context.moh.problems.structure
    assert structure is not None
    actual = parse_structure(structure)
    expected = [row.as_dict() for row in context.table]
    # convert index to int
    for row in expected:
        row["index"] = int(row["index"])

    # Convert both lists to sets of tuples for easy comparison
    actual_set = {(a["problem"], a["variable"], a["index"]) for a in actual}
    expected_set = {(e["problem"], e["variable"], e["index"]) for e in expected}

    missing = expected_set - actual_set
    extra = actual_set - expected_set

    assert not missing, f"Missing entries: {missing}"
    assert not extra,   f"Unexpected entries: {extra}"
