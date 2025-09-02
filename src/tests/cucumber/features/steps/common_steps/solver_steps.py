# Test steps definitions specific to antares-solver

import os
import re
import shutil
import subprocess
import tempfile
from pathlib import Path

import numpy as np
from behave import *
from common_steps.assertions import *
from common_steps.solver_input_handler import solver_input_handler
from common_steps.solver_output_handler import solver_output_handler

from features.steps.common_steps.assertions import assert_double_close
from features.steps.common_steps.modeler_output_handler import modeler_output_handler

NB_HOURS_IN_WEEK = 168
NB_DAYS_IN_WEEK = 7


def create_temporary_copy(path):
    temp_path = tempfile.TemporaryDirectory().name
    shutil.copytree(str(path), temp_path)
    return Path(temp_path)


@given('the solver study path is "{string}"')
def solver_study_path_is(context, string):
    context.study_path = Path(context.config.userdata["resources-path"]) / Path(string.replace("/", os.sep))
    init_simulation(context)


@given('the solver study path is a copy of "{string}"')
def solver_study_path_is(context, string):
    path = Path(context.config.userdata["resources-path"]) / Path(string.replace("/", os.sep))
    context.study_path = create_temporary_copy(path)
    context.tmp_workdir = context.study_path
    init_simulation(context)


@given('the transmission-capacities of link "{link}" are set to "{value}"')
def change_transmission_capacities(context, link, value):
    file_path = context.study_path / "input" / "links" / link / "properties.ini"
    context.sih.set_value(variable="transmission-capacities", value=value, file_path=file_path)


@when('I run antares simulator')
def run_antares(context):
    context.named_mps_problems = False
    context.parallel = False
    run_simulation(context)


@then('the simulation succeeds')
def simu_success(context):
    assert context.return_code == 0, f"Process failed with return code {context.return_code}: \nSTDOUT: \n{context.logs_out} \n STDERR: \n{context.logs_err}"


@then('the simulation fails')
def simu_success(context):
    assert context.return_code != 0, f"Process ended with return code {context.return_code}: \nSTDOUT: \n{context.logs_out} \n STDERR: \n{context.logs_err}"


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
    assert_double_close(one_year_value, context.soh.get_annual_system_cost()["EXP"], 0.00001)
    assert_double_close(0, context.soh.get_annual_system_cost()["STD"], 0.00001)
    assert_double_close(one_year_value, context.soh.get_annual_system_cost()["MIN"], 0.00001)
    assert_double_close(one_year_value, context.soh.get_annual_system_cost()["MAX"], 0.00001)


@then(
    'the annual system cost is {one_year_value1:g} with the linear solver {solver1} and {one_year_value2:g} with the others')
def check_annual_cost_depending_on_solver(context, one_year_value1, solver1, one_year_value2):
    if solver1 == get_linear_solver(context):
        check_annual_cost(context, one_year_value1)
    else:
        check_annual_cost(context, one_year_value2)


def get_linear_solver(context) -> str:
    if "linear-solver" in context.config.userdata:
        return context.config.userdata["linear-solver"]
    else:
        return "sirius"


def get_quadratic_solver(context) -> str:
    if "quadratic-solver" in context.config.userdata:
        return context.config.userdata["quadratic-solver"]
    else:
        return "sirius"


@then('the simulation takes less than {seconds:g} seconds')
def check_simu_time(context, seconds):
    assert context.soh.get_simu_time() <= seconds


@then('in area "{area}", during year {year:d}, loss of load lasts {lold_hours:d} hours')
def check_lold_duration(context, area, year, lold_hours):
    assert_double_close(lold_hours, context.soh.get_loss_of_load_duration_h(area, year), 0.001, "Loss of load")


@then('in area "{area}", during year {year:d}, week {week:d}, loss of load lasts {lold_hours:d} hours')
def check_lold_weekly_duration(context, area, year, week, lold_hours):
    assert_double_close(lold_hours, context.soh.get_loss_of_load_weekly_duration_h(area, year, week), 0.001,
                        "Loss of load")


@then('in area "{area}", during year {year:d}, total spilled energy is {value:g} MWh')
def check_spilled_energy_value(context, area, year, value):
    assert_double_close(value, context.soh.get_spilled_energy_mwh(area, year), 0.001, "Spilled energy")


@then('in area "{area}", unsupplied energy on "{date}" of year {year:d} is of {unsupplied_energy_value:g} MW')
def check_unsupplied_energy_value_for_date(context, area, date, year, unsupplied_energy_value):
    actual_unsp_energ = context.soh.get_unsupplied_energy_mwh(area, year, date)
    assert_double_close(unsupplied_energy_value, actual_unsp_energ, 0.001, "Unsupplied energy")


@then('in area "{area}", during year {year:d}, total unsupplied energy is {unsupplied_energy_value:g} MWh')
def check_unsupplied_energy_value(context, area, year, unsupplied_energy_value):
    assert_double_close(unsupplied_energy_value, context.soh.get_unsupplied_energy_mwh(area, year), 0.001,
                        "Unsupplied energy")


@then('in area "{area}", during year {year:d}, total hydro production is {value:g} MWh')
def check_hydro_production_value(context, area, year, value):
    assert_double_close(value, context.soh.get_hydro_production_mwh(area, year), 0.001, "Hydro production")


@then('in area "{area}", during year {year:d}, total hydro pumping is {value:g} MWh')
def check_hydro_pumping_value(context, area, year, value):
    assert_double_close(value, context.soh.get_hydro_pumping_mwh(area, year), 0.001, "Hydro pumping")


@then('in area "{area}", during year {year:d}, total balance is {value:g} MWh')
def check_balance_value(context, area, year, value):
    assert_double_close(value, context.soh.get_balance_mwh(area, year), 0.001, "Balance")


@then('in area "{area}", during year {year:d}, "{prod_name}" produces {value:g} MWh')
def check_production_value(context, area, year, prod_name, value):
    actual_prod = np.sum(context.soh.get_hourly_prod_mwh(area, year, prod_name))
    assert_double_close(value, actual_prod, 0.001, "Production")


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


@then(
    'in area "{area}", during year {year:d}, hourly production of "{prod_name}" for hour {hour:d} is equal to {expected_prod:d} MWh')
def check_prod_for_specific_year_hour(context, area, year, prod_name, hour, expected_prod):
    actual_hourly_prod = context.soh.get_hourly_prod_mwh(area, year, prod_name)[hour]
    assert expected_prod == actual_hourly_prod


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


@then("the annual results are")
def check_annual_results(context):
    for row in context.table:
        area = row["area"]
        year = int(row["year"])
        if should_check(row, "hydro production"):
            check_hydro_production_value(context, area, year, float(row["hydro production"]))
        if should_check(row, "hydro pumping"):
            check_hydro_pumping_value(context, area, year, float(row["hydro pumping"]))
        if should_check(row, "balance"):
            check_balance_value(context, area, year, float(row["balance"]))
        if should_check(row, "spilled energy"):
            check_spilled_energy_value(context, area, year, float(row["spilled energy"]))
        if should_check(row, "unsupplied energy"):
            check_unsupplied_energy_value(context, area, year, float(row["unsupplied energy"]))


@then("simulation tables match the references")
def check_simulation_tables(context):
    assert context.sih.get_optim1_simulation_table() == context.soh.get_optim1_simulation_table(), "first optimisation simulation table does not match the reference"
    ref_simulation_table2 = context.sih.get_optim2_simulation_table()
    if ref_simulation_table2:
        assert ref_simulation_table2 == context.soh.get_optim2_simulation_table(), "second simulation table does not match the reference"
def should_check(row, key):
    return key in row.headings and len(row[key]) > 0


def run_simulation(context):
    command = build_antares_solver_command(context)
    print(f"Running command: {command}")
    process = subprocess.Popen(command, stdout=subprocess.PIPE, stderr=subprocess.DEVNULL)
    out, err = process.communicate()
    if out:
        context.logs_out = out.decode("utf-8")
    else:
        context.logs_out = ""
    if err:
        context.logs_err = err.decode("utf-8")
    else:
        context.logs_err = ""
    context.output_path = parse_output_folder_from_logs(out)
    context.return_code = process.returncode
    context.soh = solver_output_handler(context.output_path, context.mode)
    # for hybrid studies:
    simulation_table = Path(context.output_path) / "simulation_table--optim-nb-1.csv"
    if simulation_table.exists():
        context.moh = modeler_output_handler(simulation_table)


def init_simulation(context):
    sih = solver_input_handler(context.study_path)
    # read metadata
    context.nbyears = int(sih.get_value(variable="nbyears", file_nick_name="general"))
    context.mode = sih.get_value(variable="mode", file_nick_name="general").lower()
    # activate year-by-year results  # TODO : remove this and update studies instead
    sih.set_parameter_value(variable="synthesis", value="true", file_nick_name="general")
    sih.set_parameter_value(variable="year-by-year", value="true", file_nick_name="general")
    context.sih = sih


def build_antares_solver_command(context):
    command = [context.config.userdata["antares-solver"], "-i", str(context.study_path)]
    command.append('--linear-solver=' + get_linear_solver(context))
    command.append('--quadratic-solver=' + get_quadratic_solver(context))

    if context.named_mps_problems:
        command.append('--named-mps-problems')
    if context.parallel:
        command.append('--force-parallel=4')
    return command


def parse_output_folder_from_logs(logs: bytes) -> str:
    for line in logs.splitlines():
        if b'Output folder : ' in line:
            return line.split(b'Output folder : ')[1].decode('ascii')
    raise LookupError("Could not parse output folder in output logs")


def make_daily_values_from_a_string(days: str):
    list_daily_values = [float(number) for number in re.findall(r'\d+', days)]
    assert len(list_daily_values) == NB_DAYS_IN_WEEK, "7 daily values expected, %d given" % len(list_daily_values)
    return list_daily_values


def check_week_ts_has_daily_values(week_ts, list_daily_values):
    split_ts = np.array_split(week_ts, NB_DAYS_IN_WEEK)
    for day, daily_ts in enumerate(split_ts):
        assert np.allclose(daily_ts, list_daily_values[day], atol=1e-2), \
            "day %d : all hourly values do not equal %.2f" % (day, list_daily_values[day])


def extract_week_ts(ts, week):
    assert week >= 1, "week should be greater than 1"
    assert ts.size >= 168, "hourly values should have at least 168, it has %d" % ts.size
    week_ts = ts[(week - 1) * NB_HOURS_IN_WEEK:week * NB_HOURS_IN_WEEK]
    return week_ts


@then('in area "{area}", week {week:d}, year {year:d}, daily mingens for cluster "{cluster}" are {days}')
def check_thermal_cluster_min_gen_for_week(context, area, week, year, cluster, days):
    ts = context.soh.min_gen_for_thermal_cluster(area, year, cluster)
    list_daily_values = make_daily_values_from_a_string(days)
    week_ts = extract_week_ts(ts, week)
    check_week_ts_has_daily_values(week_ts, list_daily_values)


@then('in area "{area}", year {year:d} and hour {hour:d}, withdrawal for short-term storage "{sts}" is {expected:d}')
def check_sts_withdrawal(context, area, sts, year, hour, expected):
    actual = context.soh.withdrawal_for_sts(area, year, sts)[hour]
    if expected != actual:
        print(
            f"Expected withdrawal for STS {sts} in area {area}, year {year}, hour {hour} is {expected}, but got {actual}")
    assert expected == actual


@then('in area "{area}", year {year:d} and hour {hour:d}, injection for short-term storage "{sts}" is {expected:d}')
def check_sts_injection(context, area, sts, year, hour, expected):
    actual = context.soh.injection_for_sts(area, year, sts)[hour]
    if expected != actual:
        print(
            f"Expected injection for STS {sts} in area {area}, year {year}, hour {hour} is {expected}, but got {actual}")
    assert expected == actual


@then('in area "{area}", year {year:d} and hour {hour:d}, level for short-term storage "{sts}" is {expected:d}')
def check_sts_level(context, area, sts, year, hour, expected):
    actual = context.soh.level_for_sts(area, year, sts)[hour]
    if expected != actual:
        print(
            f"Expected level for STS {sts} in area {area}, year {year}, hour {hour} is {expected}, but got {actual}")
    assert expected == actual


@then('in area "{area}", year {year:d}, no mingens for cluster "{cluster}"')
def check_no_mingen_column_for_cluster(context, area, year, cluster):
    column_names = list(context.soh.details_hourly_for_cluster(area, year, cluster).columns)
    assert "MinGen - MWh" not in column_names, "cluster %s should not be in file details" % cluster


# Unused for now
@then(
    'in area "{area}", min gen for thermal cluster "{cluster_name}" on hour {hour:d} of year {year:d} is : {expected_value:g} MW')
def check_thermal_cluster_min_gen_for_hour(context, area, cluster_name, hour, year, expected_value):
    actual_value = context.soh.min_gen_for_thermal_cluster_at_hour(area, year, hour, cluster_name)
    assert_double_close(expected_value, actual_value, 0.001)
