from behave import *
import xpress as xp
import os
from common_steps.assertions import *


def get_problem_data(mps_file: str):
    prob = xp.problem()
    prob.read(mps_file)

    rows = prob.getAttrib("rows")
    cols = prob.getAttrib("cols")
    data = {
        "rows": rows,
        "cols": cols,
        "vars": {},
        "cons": {}
    }

    col_names = prob.getnamelist(2, 0, cols - 1) if cols > 0 else []
    obj_coeffs = []
    lower_bounds = []
    upper_bounds = []
    if cols > 0:
        prob.getobj(obj_coeffs, 0, cols - 1)
    if cols > 0:
        prob.getlb(lower_bounds, 0, cols - 1)
    if cols > 0:
        prob.getub(upper_bounds, 0, cols - 1)
    print(f"Cols : {data["cols"]}")
    print(f"Col name size: {len(col_names)}\n")
    for i in range(data["cols"]):
        name = col_names[i]
        data["vars"][name] = {
            "obj": obj_coeffs[i],
            "lb": lower_bounds[i],
            "ub": upper_bounds[i]
        }

    row_names = prob.getnamelist(1, 0, rows - 1) if rows > 0 else []
    rhs = []
    if rows > 0:
        prob.getrhs(rhs, 0, rows - 1)
        row_types = []
        prob.getrowtype(row_types, 0, rows - 1)
        for i in range(data["rows"]):
            name = row_names[i]
            data["cons"][name] = {
                "rhs": rhs[i],
                "type": row_types[i]
            }
    return data


def problems_are_same(expected_data, actual_data):
    errors = []
    if expected_data["rows"] != actual_data["rows"]:
        errors.append(f"Number of rows does not match: expected={expected_data['rows']}, actual={actual_data['rows']}")
    if expected_data["cols"] != actual_data["cols"]:
        errors.append(
            f"Number of columns does not match: expected={expected_data['cols']}, actual={actual_data['cols']}")

    if len(expected_data["vars"]) != len(actual_data["vars"]):
        errors.append(
            f"Number of variables does not match: expected={len(expected_data['vars'])}, actual={len(actual_data['vars'])}")
    else:
        for name, var_data in expected_data["vars"].items():
            if name not in actual_data["vars"]:
                errors.append(f"Variable '{name}' not found in actual data")
                continue
            actual_var_data = actual_data["vars"][name]
            error = check_double_close(var_data["obj"], actual_var_data["obj"], 1e-6, f"Var '{name}' obj")
            if error:
                errors.append(error)
            error = check_double_close(var_data["lb"], actual_var_data["lb"], 1e-6, f"Var '{name}' lb")
            if error:
                errors.append(error)
            error = check_double_close(var_data["ub"], actual_var_data["ub"], 1e-6, f"Var '{name}' ub")
            if error:
                errors.append(error)

    if len(expected_data["cons"]) != len(actual_data["cons"]):
        errors.append(
            f"Number of constraints does not match: expected={len(expected_data['cons'])}, actual={len(actual_data['cons'])}")
    else:
        for name, con_data in expected_data["cons"].items():
            if name not in actual_data["cons"]:
                errors.append(f"Constraint '{name}' not found in actual data")
                continue
            actual_con_data = actual_data["cons"][name]
            error = check_double_close(con_data["rhs"], actual_con_data["rhs"], 1e-6, f"Con '{name}' rhs")
            if error:
                errors.append(error)
            if con_data["type"] != actual_con_data["type"]:
                errors.append(
                    f"Con '{name}' type does not match: expected={con_data['type']}, actual={actual_con_data['type']}")
    return errors


@step('the master problem is as expected')
def master_is_same(context):
    expected_mps_path = os.path.join(context.study_path, "expected_outputs", "master.mps")
    actual_mps_path = os.path.join(context.output_path, "master.mps")

    expected_data = get_problem_data(expected_mps_path)
    actual_data = get_problem_data(actual_mps_path)

    errors = problems_are_same(expected_data, actual_data)
    if errors:
        raise AssertionError("\n".join(errors))


@step('the sub problems are as expected')
def subproblems_are_same(context):
    all_errors = []
    for row in context.table:
        sub_problem_name = row["Sub problem name (mps file)"]
        expected_mps_path = os.path.join(context.study_path, "expected_outputs", sub_problem_name)
        actual_mps_path = os.path.join(context.output_path, sub_problem_name)

        expected_data = get_problem_data(expected_mps_path)
        actual_data = get_problem_data(actual_mps_path)
        print(f"Comparing sub problem: {sub_problem_name}")
        errors = problems_are_same(expected_data, actual_data)
        if errors:
            all_errors.append(f"Subproblem '{sub_problem_name}' has errors:")
            all_errors.extend(errors)
    if all_errors:
        raise AssertionError("\n".join(all_errors))


def parse_structure_file(file_path: str):
    """
    Parses a structure file and returns a dictionary of problems.
    The file format is expected to be: problem_name candidate_name value
    """
    problems = {}
    if not os.path.exists(file_path):
        return problems

    with open(file_path, 'r') as f:
        for line in f:
            line = line.strip()
            if not line:
                continue
            parts = line.split()
            if len(parts) != 3:
                continue
            problem_name, candidate_name, value = parts
            if problem_name not in problems:
                problems[problem_name] = {}
            problems[problem_name][candidate_name] = value
    return problems


@step('the structure are exported properly')
def structure_is_exported_properly(context):
    expected_structure_path = os.path.join(context.study_path, "expected_outputs", "structure.txt")
    actual_structure_path = os.path.join(context.output_path, "structure.txt")

    expected_problems = parse_structure_file(expected_structure_path)
    actual_problems = parse_structure_file(actual_structure_path)

    assert expected_problems.keys() == actual_problems.keys(), "Problem names do not match"

    for problem_name, expected_candidates in expected_problems.items():
        actual_candidates = actual_problems[problem_name]
        assert expected_candidates == actual_candidates, f"Candidates for problem {problem_name} do not match"
