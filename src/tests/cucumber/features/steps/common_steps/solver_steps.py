# Test steps definitions specific to antares-solver

import os
import shutil
import subprocess
import tempfile
from pathlib import Path

from behave import *
from common_steps.assertions import *
from common_steps.solver_input_handler import solver_input_handler
from common_steps.solver_output_handler import solver_output_handler

from features.steps.common_steps.assertions import assert_double_close


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
    assert_double_close(lold_hours , context.soh.get_loss_of_load_duration_h(area, year), 0.001)


@then('in area "{area}", during year {year:d}, total spilled energy is {value:g} MWh')
def check_spilled_energy_value(context, area, year, value):
    assert_double_close(value ,context.soh.get_spilled_energy_mwh(area, year), 0.001)


@then('in area "{area}", unsupplied energy on "{date}" of year {year:d} is of {unsupplied_energy_value:g} MW')
def check_unsupplied_energy_value_for_date(context, area, date, year, unsupplied_energy_value):
    actual_unsp_energ = context.soh.get_unsupplied_energy_mwh(area, year, date)
    assert_double_close(unsupplied_energy_value, actual_unsp_energ, 0.001)


@then('in area "{area}", during year {year:d}, total unsupplied energy is {unsupplied_energy_value:g} MWh')
def check_unsupplied_energy_value(context, area, year, unsupplied_energy_value):
    assert_double_close(unsupplied_energy_value, context.soh.get_unsupplied_energy_mwh(area, year), 0.001)


@then('in area "{area}", during year {year:d}, total hydro production is {value:g} MWh')
def check_spilled_energy_value(context, area, year, value):
    assert_double_close(value, context.soh.get_hydro_production_mwh(area, year), 0.001)


@then('in area "{area}", during year {year:d}, total hydro pumping is {value:g} MWh')
def check_spilled_energy_value(context, area, year, value):
    assert_double_close(value, context.soh.get_hydro_pumping_mwh(area, year), 0.001)


@then('in area "{area}", during year {year:d}, total balance is {value:g} MWh')
def check_spilled_energy_value(context, area, year, value):
    assert_double_close(value, context.soh.get_balance_mwh(area, year), 0.001)


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


def run_simulation(context):
    command = build_antares_solver_command(context)
    print(f"Running command: {command}")
    process = subprocess.Popen(command, stdout=subprocess.PIPE, stderr=subprocess.DEVNULL)
    out, err = process.communicate()
    context.output_path = parse_output_folder_from_logs(out)
    context.return_code = process.returncode
    context.soh = solver_output_handler(context.output_path)


def init_simulation(context):
    sih = solver_input_handler(context.study_path)
    # read metadata
    context.nbyears = int(sih.get_value(variable="nbyears", file_nick_name="general"))
    # activate year-by-year results  # TODO : remove this and update studies instead
    sih.set_parameter_value(variable="synthesis", value="true", file_nick_name="general")
    sih.set_parameter_value(variable="year-by-year", value="true", file_nick_name="general")
    context.sih = sih


def build_antares_solver_command(context):
    command = [context.config.userdata["antares-solver"], "-i", str(context.study_path)]
    solver = "sirius"
    if "solver" in context.config.userdata:
        solver = context.config.userdata["solver"]
    command.append('--solver=' + solver)

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