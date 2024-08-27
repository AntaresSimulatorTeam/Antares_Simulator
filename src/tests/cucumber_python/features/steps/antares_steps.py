from behave import *
from antares_utils import *
from assertions import *
import os

@given('the study path is "{string}"')
def study_path_is(context, string):
    context.study_path = os.path.join("..", "resources", "Antares_Simulator_Tests" , string)

@when('I run antares simulator')
def run_antares(context):
    context.use_ortools = True
    context.ortools_solver = "sirius"
    context.named_mps_problems = False
    context.parallel = False
    run_simulation(context)

@then('the simulation succeeds')
def simu_success(context):
    assert context.return_code == 0

@then('the simulation fails')
def simu_success(context):
    assert context.return_code != 0

@then('the expected value of the annual system cost is {value}')
def check_annual_cost_expected(context, value):
    assert_double_close(float(value), get_annual_system_cost(context)["EXP"], 0.01)

@then('the minimum annual system cost is {value}')
def check_annual_cost_min(context, value):
    assert_double_close(float(value), get_annual_system_cost(context)["MIN"], 0.01)

@then('the maximum annual system cost is {value}')
def check_annual_cost_max(context, value):
    assert_double_close(float(value), get_annual_system_cost(context)["MAX"], 0.01)

@then('the annual system cost is')
def check_annual_cost(context):
    for row in context.table:
        assert_double_close(float(row["EXP"]), get_annual_system_cost(context)["EXP"], 0.01)
        assert_double_close(float(row["MIN"]), get_annual_system_cost(context)["MIN"], 0.01)
        assert_double_close(float(row["MAX"]), get_annual_system_cost(context)["MAX"], 0.01)

def get_annual_system_cost(context):
    if context.annual_system_cost is None:
        context.annual_system_cost = parse_annual_system_cost(context.output_path)
    return context.annual_system_cost