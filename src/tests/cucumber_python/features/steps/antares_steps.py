from behave import *
from antares_utils import *
from assertions import *
import os

@given('the study path is "{string}"')
def study_path_is(context, string):
    context.study_path = os.path.join("..", "resources", "Antares_Simulator_Tests" , string)

@when('I run antares simulator')
def run_antares(context):
    file = open('latest_binary_dir.txt', 'r')
    context.solver_path = os.path.join(file.readline(), "solver", "antares-solver")
    context.raise_exception_on_failure = True
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
def check_annual_cost_expected(context, value : float):
    actual_value = float(read_output_file_and_get_value_for(context, "annualSystemCost.txt", "EXP"))
    assert_double_close(float(value), actual_value, 0.01)

@then('the minimum annual system cost is {value}')
def check_annual_cost_expected(context, value : float):
    actual_value = float(read_output_file_and_get_value_for(context, "annualSystemCost.txt", "MIN"))
    assert_double_close(float(value), actual_value, 0.01)

@then('the maximum annual system cost is {value}')
def check_annual_cost_expected(context, value : float):
    actual_value = float(read_output_file_and_get_value_for(context, "annualSystemCost.txt", "MAX"))
    assert_double_close(float(value), actual_value, 0.01)
