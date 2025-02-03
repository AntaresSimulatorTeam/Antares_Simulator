# Gherkins test steps definitions

import os
import pathlib

from behave import *

from common_steps.assertions import *
from common_steps.simulator_utils import run_simulation
from common_steps.modeler_utils import run_modeler

from features.steps.common_steps.assertions import assert_double_close


@given('the study path is "{string}"')
def study_path_is(context, string):
    context.study_path = os.path.join(context.config.userdata["resources-path"], string.replace("/", os.sep))


@when('I run antares simulator')
def run_antares(context):
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


@then('the expected value of the annual system cost is {value:g}')
def check_annual_cost_expected(context, value):
    assert_double_close(value, context.soh.get_annual_system_cost()["EXP"], 0.001)


@then('the minimum annual system cost is {value:g}')
def check_annual_cost_min(context, value):
    assert_double_close(value, context.soh.get_annual_system_cost()["MIN"], 0.001)


@then('the maximum annual system cost is {value:g}')
def check_annual_cost_max(context, value):
    assert_double_close(value, context.soh.get_annual_system_cost()["MAX"], 0.001)


@then('the annual system cost is')
def check_annual_cost(context):
    for row in context.table:
        assert_double_close(float(row["EXP"]), context.soh.get_annual_system_cost()["EXP"], 0.001)
        assert_double_close(float(row["STD"]), context.soh.get_annual_system_cost()["STD"], 0.001)
        assert_double_close(float(row["MIN"]), context.soh.get_annual_system_cost()["MIN"], 0.001)
        assert_double_close(float(row["MAX"]), context.soh.get_annual_system_cost()["MAX"], 0.001)


@then('the annual system cost is {one_year_value:g}')
def check_annual_cost(context, one_year_value):
    assert_double_close(one_year_value, context.soh.get_annual_system_cost()["EXP"], 0.001)
    assert_double_close(0, context.soh.get_annual_system_cost()["STD"], 0.001)
    assert_double_close(one_year_value, context.soh.get_annual_system_cost()["MIN"], 0.001)
    assert_double_close(one_year_value, context.soh.get_annual_system_cost()["MAX"], 0.001)


@then('the simulation takes less than {seconds:g} seconds')
def check_simu_time(context, seconds):
    assert context.soh.get_simu_time() <= seconds


@then('in area "{area}", during year {year:d}, loss of load lasts {lold_hours:d} hours')
def check_lold_duration(context, area, year, lold_hours):
    assert lold_hours == context.soh.get_loss_of_load_duration_h(area, year)


@then('in area "{area}", unsupplied energy on "{date}" of year {year:d} is of {lold_value_mw:g} MW')
def check_lold_value(context, area, date, year, lold_value_mw):
    actual_unsp_energ = context.soh.get_unsupplied_energy_mwh(area, year, date)
    assert_double_close(lold_value_mw, actual_unsp_energ, 0.001)


@then(
    'in area "{area}", during year {year:d}, hourly production of "{prod_name}" is always {comparator_and_hourly_prod} MWh')
def check_prod_for_specific_year(context, area, year, prod_name, comparator_and_hourly_prod):
    expected_prod = float(comparator_and_hourly_prod.split(" ")[-1])
    actual_hourly_prod = context.soh.get_hourly_prod_mwh(area, year, prod_name)
    if "greater than" in comparator_and_hourly_prod:
        ok = actual_hourly_prod >= expected_prod
    elif "equal to" in comparator_and_hourly_prod:
        ok = actual_hourly_prod - expected_prod <= 1e-6
    else:
        raise NotImplementedError(f"Unknown comparator '{comparator_and_hourly_prod}'")
    if "zero or" in comparator_and_hourly_prod:
        ok = ok | (actual_hourly_prod == 0)
    assert ok.all()


@then('in area "{area}", hourly production of "{prod_name}" is always {comparator_and_hourly_prod} MWh')
def check_prod_for_all_years(context, area, prod_name, comparator_and_hourly_prod):
    for year in range(1, context.nbyears + 1):
        check_prod_for_specific_year(context, area, year, prod_name, comparator_and_hourly_prod)


@step('in area "{area}", during year {year:d}, total non-proportional cost is {np_cost:g}')
def check_np_cost_for_specific_year(context, area, year, np_cost):
    assert_double_close(np_cost, context.soh.get_non_proportional_cost(area, year), 1e-6)


@then('in area "{area}", the units of "{prod_name}" produce between {min_p:g} and {max_p:g} MWh hourly')
def check_pmin_pmax(context, area, prod_name, min_p, max_p):
    for year in range(1, context.nbyears + 1):
        actual_hourly_prod = context.soh.get_hourly_prod_mwh(area, year, prod_name)
        actual_n_dispatched_units = context.soh.get_hourly_n_dispatched_units(area, year, prod_name)
        assert (actual_hourly_prod <= actual_n_dispatched_units.apply(
            lambda n: n * max_p)).all(), f"max_p constraint not respected during year {year}"
        assert (actual_hourly_prod >= actual_n_dispatched_units.apply(
            lambda n: n * min_p)).all(), f"min_p constraint not respected during year {year}"


@when("I run antares modeler")
def run_antares_modeler(context):
    run_modeler(context)


@step('the optimal value of variable {var} is {value:g}')
def modeler_var_optimal_value(context, var, value):
    assert_double_close(value, context.moh.get_optimal_value(var), 1e-6)