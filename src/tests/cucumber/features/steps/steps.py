# Gherkins test steps definitions

import pathlib

from behave import *

from assertions import *
from context_utils import *
from simulator_utils import *


@given('the study path is "{string}"')
def study_path_is(context, string):
    context.study_path = os.path.join("..", "resources", "Antares_Simulator_Tests_NR", string.replace("/", os.sep))


@when('I run antares simulator')
def run_antares(context):
    context.use_ortools = True
    context.ortools_solver = "sirius"
    context.named_mps_problems = False
    context.parallel = False
    run_simulation(context)


def after_feature(context, feature):
    # post-processing a test: clean up output files to avoid taking up all the disk space
    if (context.output_path != None):
        pathlib.Path.rmdir(context.output_path)


@then('the simulation succeeds')
def simu_success(context):
    assert context.return_code == 0


@then('the simulation fails')
def simu_success(context):
    assert context.return_code != 0


@then('the expected value of the annual system cost is {value}')
def check_annual_cost_expected(context, value):
    assert_double_close(float(value), get_annual_system_cost(context)["EXP"], 0.001)


@then('the minimum annual system cost is {value}')
def check_annual_cost_min(context, value):
    assert_double_close(float(value), get_annual_system_cost(context)["MIN"], 0.001)


@then('the maximum annual system cost is {value}')
def check_annual_cost_max(context, value):
    assert_double_close(float(value), get_annual_system_cost(context)["MAX"], 0.001)


@then('the annual system cost is')
def check_annual_cost(context):
    for row in context.table:
        assert_double_close(float(row["EXP"]), get_annual_system_cost(context)["EXP"], 0.001)
        assert_double_close(float(row["STD"]), get_annual_system_cost(context)["STD"], 0.001)
        assert_double_close(float(row["MIN"]), get_annual_system_cost(context)["MIN"], 0.001)
        assert_double_close(float(row["MAX"]), get_annual_system_cost(context)["MAX"], 0.001)


@then('the annual system cost is {one_year_value}')
def check_annual_cost(context, one_year_value):
    assert_double_close(float(one_year_value), get_annual_system_cost(context)["EXP"], 0.001)
    assert_double_close(0, get_annual_system_cost(context)["STD"], 0.001)
    assert_double_close(float(one_year_value), get_annual_system_cost(context)["MIN"], 0.001)
    assert_double_close(float(one_year_value), get_annual_system_cost(context)["MAX"], 0.001)


@then('the simulation takes less than {seconds} seconds')
def check_simu_time(context, seconds):
    actual_simu_time = parse_simu_time(context.output_path)
    assert actual_simu_time <= float(seconds)


@then('in area "{area}", during year {year}, loss of load lasts {lold_hours} hours')
def check_lold_duration(context, area, year, lold_hours):
    assert int(lold_hours) == get_values_hourly(context, area.lower(), int(year))["LOLD"].sum()


@then('in area "{area}", unsupplied energy on "{date}" of year {year} is of {lold_value_mw} MW')
def check_lold_value(context, area, date, year, lold_value_mw):
    actual_unsp_energ = get_values_hourly_for_specific_hour(context, area.lower(), int(year), date)["UNSP. ENRG"].sum()
    assert_double_close(float(lold_value_mw), actual_unsp_energ, 0.001)


@then(
    'in area "{area}", during year {year}, hourly production on "{prod_name}" is always {comparator_and_hourly_prod} MWh')
def check_prod_for_specific_year(context, area, year, prod_name, comparator_and_hourly_prod):
    expected_prod = float(comparator_and_hourly_prod.split(" ")[-1])
    actual_hourly_prod = get_details_hourly(context, area.lower(), int(year))[prod_name]
    if "greater than" in comparator_and_hourly_prod:
        ok = actual_hourly_prod >= expected_prod
    elif "equal to" in comparator_and_hourly_prod:
        ok = actual_hourly_prod - expected_prod <= 1e-6
    else:
        raise NotImplementedError(f"Unknown comparator '{comparator_and_hourly_prod}'")
    if "zero or" in comparator_and_hourly_prod:
        ok = ok | (actual_hourly_prod == 0)
    assert ok.all()


@then('in area "{area}", hourly production on "{prod_name}" is always {comparator_and_hourly_prod} MWh')
def check_prod_for_all_years(context, area, prod_name, comparator_and_hourly_prod):
    for year in range(1, context.nbyears + 1):
        check_prod_for_specific_year(context, area, year, prod_name, comparator_and_hourly_prod)


@then(
    'in area "{area}", unit "{prod_name}" respects a minimum up duration of {up_duration} hours, and a minimum down duration of {down_duration} hours')
def check_min_up_down_duration(context, area, prod_name, up_duration, down_duration):
    for year in range(1, context.nbyears + 1):
        actual_hourly_prod = get_details_hourly(context, area.lower(), year)[prod_name]
        min_up, min_down = compute_min_up_and_down_durations(actual_hourly_prod)
        assert min_up >= float(up_duration)
        assert min_down >= float(down_duration)
