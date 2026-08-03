# Copyright 2007-2026, RTE (https://www.rte-france.com)
# SPDX-License-Identifier: MPL-2.0

import math
# Test steps definitions specific to antares-modeler

import os
import subprocess

from behave import *
from common_steps.assertions import *
from common_steps.invest_problems import read_invest_problems
from common_steps.simulation_table import SimulationTable
from common_steps.simulation_table_reader import (
    OutputFormat,
    make_simu_table_reader,
)
from shared_utils import mps_utils as mpu
from pathlib import Path


@given('the modeler study path is "{string}"')
def modeler_study_path_is(context, string):
    context.study_path = os.path.join(context.config.userdata["resources-path"], string.replace("/", os.sep))


@when("I run antares modeler")
def run_antares_modeler(context):
    context.outputFormat = OutputFormat.CSV
    run_modeler(context)

@when("I run antares problem generator")
def run_antares_problem_generator(context):
    run_problem_generator(context)

@when("I run antares modeler with parquet")
def run_antares_modeler_parquet(context):
    context.outputFormat = OutputFormat.PARQUET
    run_modeler(context)

@step('the objective value is {value:g}')
def modeler_obj_value(context, value):
    assert_double_close(value, context.simu_table.get_objective_value(), 1e-5)


@step('the objective value is greater than {lb:g} and lower than {ub:g}')
def modeler_obj_value(context, lb, ub):
    assert lb <= context.simu_table.get_objective_value() <= ub, \
        f"Objective value is not inside expected range: {context.simu_table.get_objective_value()}"


@step('the modeler outputs contain the following entries')
def modeler_output_values(context):
    check_simulation_table_content(context, 1e-6)


@step('the modeler outputs contain the following entries with relative tolerance {tolerance:g}')
def modeler_output_values_with_tolerance(context, tolerance):
    check_simulation_table_content(context, tolerance)

def check_simulation_table_content(context, tolerance):
    expected_entries = read_expected_entries(context.table)
    check_st_entries(context.simu_table, expected_entries, tolerance)


def read_int_range(row, key: str):
    if row[key] != "":
        array = row[key].split("-")
        start = int(array[0])
        end = int(array[1]) if len(array) == 2 else start
        return range(start, end + 1)
    else:
        return [math.nan]


def read_expected_entries(table):
    """Read expected entries from a Gherkin table.

    Returns a list of dicts, each with keys:
        component, output, timestep (range), block (range or None), scenario (range or None), value
    """
    entries = []
    for row in table:
        entry = {
            "component": row["component"],
            "output": row["output"],
            "timestep": read_int_range(row, "timestep"),
            "value": float(row["value"]),
        }
        entry["scenario"] = (
            read_int_range(row, "scenario")
            if "scenario" in table.headings
            else [0]
        )
        entry["block"] = (
            read_int_range(row, "block")
            if "block" in table.headings
            else [math.nan]
        )
        entries.append(entry)
    return entries


def check_st_entries(simulation_table: SimulationTable, expected_entries, tolerance):
    """Check that the simulation table contains all expected entries."""
    for entry in expected_entries:
        component = entry["component"]
        output = entry["output"]
        value = entry["value"]
        for block in entry["block"]:
            for scenario in entry["scenario"]:
                for ts in entry["timestep"]:
                    actual = simulation_table.get_entry(
                        component, output, block, ts, scenario
                    )
                    assert_double_close(value, actual, tolerance)


def run_executable(context, command) -> bool:
    context.output_path = os.path.join(context.study_path, "output")
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

    context.return_code = process.returncode

    if process.returncode != 0:
        print("*********************** Begin stdout ***********************")
        print(out.replace(b'\r\n', b'\n').decode('utf-8'))
        print("*********************** End stdout ***********************")

        print("*********************** Begin stderr ***********************")
        print(err.replace(b'\r\n', b'\n').decode('utf-8'))
        print("*********************** End stderr ***********************")
        return False
    return True

def run_modeler(context):
    modeler_cmd = build_antares_modeler_command(context)
    if not run_executable(context, modeler_cmd):
        return

    output_format = getattr(context, "outputFormat", OutputFormat.CSV)
    file_pattern = f"simulation-table*.{output_format.value}"
    output_path = Path(parse_output_folder_from_logs(context.logs_out))
    reader_factory = make_simu_table_reader(output_path, output_format, file_pattern)
    context.simu_table = SimulationTable(reader_factory())

    context.invest_pb = read_invest_problems(Path(parse_output_folder_from_logs(context.logs_out)))


def run_problem_generator(context):
    pb_generator_cmd = build_antares_problem_generator_command(context)
    if not run_executable(context, pb_generator_cmd):
        return

    context.invest_pb = read_invest_problems(Path(parse_output_folder_from_logs(context.logs_out)))

def build_antares_modeler_command(context):
    command = [context.config.userdata["antares-modeler"], str(context.study_path)]
    if getattr(context, "outputFormat", OutputFormat.CSV) == OutputFormat.PARQUET:
        command.append("--parquet")
    return command


def build_antares_problem_generator_command(context):
    command = [context.config.userdata["antares-problem-generator"], str(context.study_path)]
    return command

# TODO: code duplicated with solver steps
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
    assert context.invest_pb is not None and context.invest_pb.master is not None
    check_variables(context, context.invest_pb.master)


@then(u'the subproblem contains the following variables')
def check_subproblem_variables(context):
    assert context.invest_pb is not None and context.invest_pb.subproblem is not None
    check_variables(context, context.invest_pb.subproblem)


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
    structure = context.invest_pb.structure
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
    assert not extra, f"Unexpected entries: {extra}"

@then(u'the system-for-views.yml is correctly exported')
def check_system_for_views_exported(context):
    output_folder = Path(parse_output_folder_from_logs(context.logs_out))
    system_for_views_file = output_folder / "system-for-views.yml"
    assert system_for_views_file.exists(), f"{system_for_views_file} does not exist"

    system_input_path = Path(context.study_path) / "input" / "system.yml"
    system_input_file = system_input_path.open()
    assert system_input_file.read() == system_for_views_file.read_text(), "system-for-views.yml content does not match system.yml"
