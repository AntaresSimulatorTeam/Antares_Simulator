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
    prob.getobj(obj_coeffs, 0, cols - 1)
    lower_bounds = []
    prob.getlb(lower_bounds, 0, cols - 1)
    upper_bounds = []
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
    if data["rows"] > 0:
        rhs = []
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


@step('the master problem is as expected')
def master_is_same(context):
    expected_mps_path = os.path.join(context.study_path, "expected_outputs", "master.mps")
    actual_mps_path = os.path.join(context.output_path, "master.mps")

    expected_data = get_problem_data(expected_mps_path)
    actual_data = get_problem_data(actual_mps_path)

    assert expected_data["rows"] == actual_data["rows"], "Number of rows does not match"
    assert expected_data["cols"] == actual_data["cols"], "Number of columns does not match"

    assert len(expected_data["vars"]) == len(actual_data["vars"])
    for name, var_data in expected_data["vars"].items():
        assert name in actual_data["vars"]
        actual_var_data = actual_data["vars"][name]
        assert_double_close(var_data["obj"], actual_var_data["obj"], 1e-6)
        assert_double_close(var_data["lb"], actual_var_data["lb"], 1e-6)
        assert_double_close(var_data["ub"], actual_var_data["ub"], 1e-6)

    assert len(expected_data["cons"]) == len(actual_data["cons"])
    for name, con_data in expected_data["cons"].items():
        assert name in actual_data["cons"]
        actual_con_data = actual_data["cons"][name]
        assert_double_close(con_data["rhs"], actual_con_data["rhs"], 1e-6)
        assert con_data["type"] == actual_con_data["type"]
