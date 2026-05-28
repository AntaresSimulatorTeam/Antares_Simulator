# Test steps definitions specific to antares-solver

import os
import re
import shutil
import subprocess
import tempfile
from pathlib import Path

import numpy as np
from behave import *
from common_steps.solver_input_handler import solver_input_handler
from common_steps.solver_output_handler import solver_output_handler

from common_steps.assertions import assert_double_close
from common_steps.modeler_output_handler import modeler_output_handler
from common_steps.table_compare import *
from shared_utils import mps_utils as mpu

from common_steps.parse_linear_expression import parse_linear_expression

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

@when('I replace the "{destinationPath}" file with "{originPath}"')
def replace_reserve_ini(context, destinationPath, originPath):
    destination = destinationPath.split("/")
    origin = originPath.split("/")
    input_handler = solver_input_handler(Path(context.study_path))
    input_handler.copy_reserve_ini_from_file(origin, destination)


@given('the linear solver is {solver_name}')
def set_linear_solver(context, solver_name):
    context.config.userdata["linear-solver"] = solver_name

@given('the quadratic solver is {solver_name}')
def set_quadratic_solver(context, solver_name):
    context.config.userdata["quadratic-solver"] = solver_name


def parse_options(context, options):
    context.named_mps_problems = False
    context.parallel = False
    if options is not None:
        if options.count("--named-mps-problems") > 0 or options.count("-s") > 0:
            context.named_mps_problems = True

        if options.count("--parallel") > 0:
            context.parallel = True

@when('I run antares simulator')
@when('I run antares simulator with {options}')
def run_antares(context, options=None):
    parse_options(context, options)
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
    assert context.soh.get_simu_time() <= seconds, f"Simulation time {context.soh.get_simu_time()} exceeds {seconds} seconds"


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


@then('in area "{area}", overall cost on "{date}" of year {year:d} is of {overall_cost_value:g} Euro')
def check_overall_cost_for_date(context, area, date, year, overall_cost_value):
    actual_overall_cost = context.soh.get_overall_cost_eur(area, year, date)
    assert_double_close(overall_cost_value, actual_overall_cost, 0.001, "Overall cost")


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


@then('in area "{area}", battery level on "{date}" of year {year:d} is {lold_value_mw:g} MWh')
def check_lold_value(context, area, date, year, lold_value_mw):
    actual_battery_level = context.soh.get_battery_level_mwh(area, year, date)
    assert_double_close(lold_value_mw, actual_battery_level, 0.001)

@then('in area "{area}", during year {year:d}, "{prod_name}" produces {value:g} MWh')
def check_production_value(context, area, year, prod_name, value):
    actual_prod = np.sum(context.soh.get_hourly_prod_mwh(area, year, prod_name))
    assert_double_close(value, actual_prod, 0.001, "Production")

    
@then('in area "{area}", during year {year:d}, hourly production of "{prod_name}" is always {comparator_and_hourly_prod} MWh')
def check_prod_for_specific_year(context, area, year, prod_name, comparator_and_hourly_prod):
    expected_prod = float(comparator_and_hourly_prod.split(" ")[-1])
    actual_hourly_prod = context.soh.get_hourly_prod_mwh(area, year, prod_name)
    msg = "At least one value in power production power "
    if "greater than" in comparator_and_hourly_prod:
        ok = actual_hourly_prod >= expected_prod
        if not ok.all():
            msg += f'is not superior to {expected_prod} MWh'
    elif "smaller than" in comparator_and_hourly_prod:
        ok = actual_hourly_prod <= expected_prod
        if not ok.all():
            msg += f'is not inferior to {expected_prod} MWh'
    elif "equal to" in comparator_and_hourly_prod:
        ok = (actual_hourly_prod - expected_prod).abs() <= 1e-6
        if not ok.all():
            msg += f'is not close to {expected_prod} MWh'
    else:
        raise NotImplementedError(f"Unknown comparator '{comparator_and_hourly_prod}'")
    if "zero or" in comparator_and_hourly_prod:
        ok = ok | (actual_hourly_prod == 0)
        msg += " (or null)"
    assert ok.all(), msg


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
    reference_dir = context.sih.reference_dir()
    output_dir = context.soh.output_dir()

    dirs_match, differences = compare_folders(reference_dir, output_dir)
    assert dirs_match, "Folders do not match:\n" + "\n".join(f"  - {diff}" for diff in differences)


def should_check(row, key):
    return key in row.headings and len(row[key]) > 0


def run_simulation(context):
    command = build_antares_solver_command(context)
    print(f"Running command: {command}")
    process = subprocess.Popen(command, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    out, err = process.communicate()
    try:
        if out:
            context.logs_out = out.decode('utf-8')
        else:
            context.logs_out = ""
        if err:
            context.logs_err = err.decode('utf-8')
        else:
            context.logs_err = ""
    except UnicodeDecodeError: # On windows, process communication can return another format
        if out:
            context.logs_out = out.decode('cp1252')
        else:
            context.logs_out = ""
        if err:
            context.logs_err = err.decode('cp1252')
        else:
            context.logs_err = ""
    context.output_path = parse_output_folder_from_logs(out)
    context.return_code = process.returncode
    context.soh = solver_output_handler(context.output_path, context.mode)
    # for hybrid studies:
    output_dir = Path(context.output_path)
    simulation_tables = sorted(output_dir.glob("simulation_table-*--optim-nb-1.csv"))
    if simulation_tables:
        context.moh = modeler_output_handler(simulation_tables)


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

def make_values_from_string(values: str):
    list_values = [float(number.strip()) for number in values.split(",")]
    return list_values

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
    list_daily_values = make_values_from_string(days)
    assert len(list_daily_values) == NB_DAYS_IN_WEEK, "7 daily values expected, %d given" % len(list_daily_values)
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

@then('in area "{area}", during year {year:d}, for cluster "{cluster}" and reserve "{res}", total reserve participation power is {res_part:g} MWh')
def check_res_participation_for_specific_year_and_cluster_yearly(context, area, year, res, cluster, res_part):
    assert_double_close(res_part, context.soh.get_reserve_total_participation_for_year_and_cluster(area, year, res,cluster), 1e-6)

@then('in area "{area}", during year {year:d}, for cluster "{cluster}" and reserve "{res}", total reserve participation power is inferior to {res_part:g} MWh')
def check_res_participation_for_specific_year_and_cluster_yearly_inferior(context, area, year, res, cluster, res_part):
    assert (context.soh.get_reserve_total_participation_for_year_and_cluster(area, year, res,cluster) < res_part)

@step('the message "{log}" is reported in the logs')
def ckeck_log_exists(context, log):
    for log_line in context.logs_err.splitlines():
        if log in log_line:
            return
    raise AssertionError(f"Log '{log}' is not reported in the logs")


def _initialize_multi_study_context(context):
    """Initialize context for multiple study simulations"""
    if not hasattr(context, 'multi_studies'):
        context.multi_studies = []


def _store_simulation_result(context, study_index: int):
    """Store simulation results for a given study index"""
    result = {
        'index': study_index,
        'path': context.study_path,
        'return_code': context.return_code,
        'logs_out': context.logs_out,
        'logs_err': context.logs_err,
        'output_path': context.output_path,
        'soh': context.soh,
        'moh': context.moh if hasattr(context, 'moh') else None
    }
    context.multi_studies.append(result)


def _run_study_at_index(context, study_index: int, study_path: Path):
    """Run a single study and store its results"""
    context.study_path = study_path
    init_simulation(context)
    context.named_mps_problems = False
    context.parallel = False
    run_simulation(context)
    _store_simulation_result(context, study_index)


@given('the study path {study_num:d} is "{string}"')
def nth_study_path_is(context, study_num, string):
    """Generic step to define the Nth study path"""
    _initialize_multi_study_context(context)
    study_path = Path(context.config.userdata["resources-path"]) / Path(string.replace("/", os.sep))
    assert study_path.exists(), f"Study path {study_num} ({study_path}) does not exist"

    if not hasattr(context, 'study_paths'):
        context.study_paths = []

    # Extend list if necessary to accommodate the index
    while len(context.study_paths) < study_num:
        context.study_paths.append(None)

    # Store at index (1-based to 0-based conversion)
    context.study_paths[study_num - 1] = study_path


@when('I run antares simulator on all studies')
def run_antares_on_all_studies(context):
    """Run simulator on all defined studies"""
    _initialize_multi_study_context(context)

    assert hasattr(context, 'study_paths'), "No study paths defined"
    assert len(context.study_paths) > 0, "No study paths defined"

    # Run all studies
    for idx, study_path in enumerate(context.study_paths):
        assert study_path is not None, f"Study path at index {idx} is not defined"
        _run_study_at_index(context, idx, study_path)


@then('all simulations succeed')
def all_simulations_succeed(context):
    """Check that all simulations succeeded"""
    assert hasattr(context, 'multi_studies'), "No simulations were run"
    assert len(context.multi_studies) > 0, "No simulations were run"

    failures = []
    for study in context.multi_studies:
        if study['return_code'] != 0:
            failures.append(
                f"Study {study['index'] + 1} (path: {study['path']}) failed with return code {study['return_code']}: "
                f"\nSTDOUT: \n{study['logs_out']} \n STDERR: \n{study['logs_err']}"
            )

    if failures:
        raise AssertionError("\n\n".join(failures))


@then('for each time step, all studies have the same objective value')
def compare_objective_values_all_studies(context):
    """Compare objective values across all studies to ensure they are identical"""
    assert hasattr(context, 'multi_studies'), "No simulations were run"
    assert len(context.multi_studies) > 1, f"Need at least 2 studies to compare, found {len(context.multi_studies)}"

    # Collect all objective values
    all_objectives = []
    for study in context.multi_studies:
        assert study['moh'] is not None, \
            f"Study {study['index'] + 1} (path: {study['path']}) does not have modeler outputs (simulation_table)"
        objectives = study['moh'].get_objective_values_by_block()
        all_objectives.append({
            'index': study['index'],
            'path': study['path'],
            'objectives': objectives
        })

    # Use first study as reference
    reference = all_objectives[0]
    reference_blocks = set(reference['objectives'].keys())

    # Compare each study against the reference
    all_mismatches = []
    for study_data in all_objectives[1:]:
        study_blocks = set(study_data['objectives'].keys())

        # Check blocks match
        if reference_blocks != study_blocks:
            all_mismatches.append(
                f"Study {study_data['index'] + 1} has different blocks. "
                f"Reference: {sorted(reference_blocks)}, Study: {sorted(study_blocks)}"
            )
            continue

        # Compare values for each block
        mismatches = []
        for block in sorted(reference_blocks):
            ref_value = reference['objectives'][block]
            study_value = study_data['objectives'][block]
            if not np.isclose(ref_value, study_value, rtol=1e-9, atol=1e-9):
                mismatches.append(
                    f"  Block {block}: reference={ref_value}, study {study_data['index'] + 1}={study_value}, "
                    f"diff={abs(ref_value - study_value)}"
                )

        if mismatches:
            all_mismatches.append(
                f"Study {study_data['index'] + 1} (path: {study_data['path']}) differs from reference:\n" +
                "\n".join(mismatches)
            )

    if all_mismatches:
        error_msg = "Objective values differ between studies:\n\n" + "\n\n".join(all_mismatches)
        raise AssertionError(error_msg)


@then(
    'in area "{area}", during year {year:d}, hourly value of "{var_name}" for hour {hour:d} is equal to {expected_value:d}')
def check_hourly_variable_value(context, area, year, var_name, hour, expected_value):
    actual_value = context.soh.get_hourly_value(area, year, var_name, hour)
    assert expected_value == actual_value, \
        f"Hourly value mismatch for {var_name}: expected {expected_value}, got {actual_value} (area={area}, year={year}, hour={hour})"


@then('in area "{area}", year {year:d} and hour {hour:d}, near price cap is {value:d} hours')
def check_near_price_cap(context, area, year, hour, value):
    actual = context.soh.get_npcap_hours_for_hour(area, year, hour)
    assert actual == value, f"Near price cap hours mismatch: expected {value}, got {actual}"

def week_hours_from_mps_filename(file_name):
    """
    Extract week number from MPS file name and generate 168 time steps.

    Args:
        file_name: MPS file name like 'problem-1-1--optim-nb-1.mps' or full path

    Returns:
        List of 168 time steps for the week: [(week-1)*168 + hour for hour in 0..167]

    Example:
        'problem-1-1-optim-nb-1.mps' -> week=1 -> [0, 1, 2, ..., 167]
        'problem-1-2-optim-nb-1.mps' -> week=2 -> [168, 169, 170, ..., 335]
    """
    # Extract just the filename if a path was provided
    file_name = Path(file_name).name

    # Parse file name pattern: problem-{year}-{week}-optim-nb-{number}.mps
    # Using regex to extract the week number (second number after 'problem-')
    match = re.match(r'problem-(\d+)-(\d+)--optim-nb-(\d+)\.mps', file_name)

    if not match:
        raise ValueError(f"Invalid MPS file name format: {file_name}. "
                         f"Expected format: problem-<year>-<week>--optim-nb-<number>.mps")

    year = int(match.group(1))
    week = int(match.group(2))
    optim_nb = int(match.group(3))

    # Generate 168 time steps for the week
    week_hours = [(week - 1) * 168 + hour for hour in range(168)]

    return week_hours


def extract_hour(name) -> int:
    return int(name.split("hour<")[1].split(">")[0])


@then(
    'for each weekly problem, verify the "MaxGenerationFromCapacity" constraint exists for all time steps for thermal cluster {cluster} in area {area}.')
def check_max_generation_from_capacity_exists(context, area, cluster):
    for mps_file in context.soh.get_mps_files():
        mps_problem = mpu.load_problem(str(mps_file))
        week_hours = week_hours_from_mps_filename(mps_file)
        hours_mapped_to_constraints: dict[int, str] = {}
        for c in mps_problem.get_linear_constraints():
            if c.name.startswith(f"MaxGenerationFromCapacity::area<{area}>::ThermalCluster<{cluster}>::"):
                hour = extract_hour(c.name)
                hours_mapped_to_constraints[hour] = c.name
        assert len(week_hours) == len(hours_mapped_to_constraints), \
            f"MaxGenerationFromCapacity::area<{area}>::ThermalCluster<{cluster}>:: constraint not found for all time steps in {mps_file}"
        assert week_hours == list(
            hours_mapped_to_constraints.keys()), f"MaxGenerationFromCapacity::area<{area}>::ThermalCluster<{cluster}>:: constraint does not match time steps [{week_hours[0]}, {week_hours[-1]}] in {mps_file}"

@then('for first week, area balance RHS (for area {area}) is first {values_str}, then equals constant {constant_str}')
def check_area_balance_rhs(context, area, values_str, constant_str):
    list_values = make_values_from_string(values_str)
    constant = float(constant_str)

    mps_file_path = context.soh.get_output_file_with_name("problem-1-1--optim-nb-1.mps")
    assert mps_file_path, f"No output file named problem-1-1--optim-nb-1.mps"

    mps_problem = mpu.load_problem(mps_file_path)
    balanced_constraints = [c for c in mps_problem.get_linear_constraints() if c.name.startswith(f"AreaBalance::area<{area}>")]

    failures = []

    # Checking first balance constraints have expected bounds
    first_constraints = balanced_constraints[0:len(list_values)]
    for c, value in zip(first_constraints, list_values):
        if c.lower_bound != value:
            failures.append(f"Constraint {c.name} low bound should be {str(value)}, is {c.lower_bound}")
        if c.upper_bound != value:
            failures.append(f"Constraint {c.name} up bound should be {str(value)}, is {c.upper_bound}")

    # Checking remaining balance constraints have same expected bounds
    last_constraints = balanced_constraints[len(list_values):]
    for c in last_constraints:
        if c.lower_bound != constant:
            failures.append(f"Constraint {c.name} low bound should be {constant_str}, is {c.lower_bound}")
        if c.upper_bound != constant:
            failures.append(f"Constraint {c.name} up bound should be {constant_str}, is {c.upper_bound}")

    if failures:
        raise AssertionError("Area balance RHS check failed with the following errors:\n" + "\n".join(failures))

@then(
    'enforces: DispatchableProduction {expression} < {rhs} for the thermal capacity connection between GEMS and the {cluster} thermal cluster in area {area}.')
def check_max_generation_from_capacity_constraint(context, expression, rhs, cluster, area):
    for mps_file in context.soh.get_mps_files():
        mps_problem = mpu.load_problem(str(mps_file))
        parsed_expression = parse_linear_expression(expression)
        rhs = float(rhs)
        for constr_name, row in mpu.get_constraint_matrix(mps_problem).items():
            if constr_name.startswith(f"MaxGenerationFromCapacity::area<{area}>::ThermalCluster<{cluster}>::"):
                hour = extract_hour(constr_name)
                assert len(
                    row) == 2, f"{constr_name} must have exactly two non null coefficients: DispatchableProduction-thermal_add_on.p_max<0"
                # Check coefficients
                for var_name, coeff in row.items():
                    dispatchable_production = f"DispatchableProduction::area<{area}>::ThermalCluster<{cluster}>::hour<{hour}>"

                    if var_name == dispatchable_production:
                        assert coeff == 1.0, f"the coefficient of {dispatchable_production} in {constr_name} must be = 1"
                    elif var_name in parsed_expression:
                        assert coeff == parsed_expression[
                            var_name], f"the coefficient of {var_name} in {constr_name} must be = {coeff}"
                    else:
                        raise ValueError(
                            f"{var_name} should not have coefficient in MaxGenerationFromCapacity::area<{area}>::ThermalCluster<{cluster}>::hour<{hour}>")

@then('in area "{area}", during year {year:d}, for cluster "{cluster}" and reserve "{res}", reserve participation power is always {comparator_and_res_part} MWh')
def check_res_participation_for_specific_year_and_cluster_hourly(context, area, year, res, cluster, comparator_and_res_part):
    expected_res_part = float(comparator_and_res_part.split(" ")[-1])
    actual_hourly_prod = context.soh.get_hourly_res_part_mwh(area, year, res + "_" + cluster)
    msg = "At least one value in reserve participation power "
    if "greater than" in comparator_and_res_part:
        ok = actual_hourly_prod >= expected_res_part
        if not ok.all():
            msg += f'is not superior to {expected_res_part} MWh'
    elif "smaller than" in comparator_and_res_part:
        ok = actual_hourly_prod <= expected_res_part
        if not ok.all():
            msg += f'is not inferior to {expected_res_part} MWh'
    elif "equal to" in comparator_and_res_part:
        ok = (actual_hourly_prod - expected_res_part).abs() <= 1e-6
        if not ok.all():
            msg += f'is not close to {expected_res_part} MWh'
    else:
        raise NotImplementedError(f"Unknown comparator '{comparator_and_res_part}'")
    if "zero or" in comparator_and_res_part:
        ok = ok | (actual_hourly_prod == 0)
        msg += " (or null)"
    assert ok.all(), msg
    
@then('in area "{area}", during year {year:d}, for group "{group}" and reserve "{res}", reserve participation power is always {comparator_and_res_part} MWh')
def check_res_participation_for_specific_year_and_group_hourly(context, area, year, res, group, comparator_and_res_part):
    expected_res_part = float(comparator_and_res_part.split(" ")[-1])
    actual_hourly_prod = context.soh.get_hourly_reserve_group_energy(area, year, res, group)
    msg = "At least one value in reserve participation power to group " + group
    if "greater than" in comparator_and_res_part:
        ok = actual_hourly_prod >= expected_res_part
        if not ok.all():
            msg += f'is not superior to {expected_res_part} MWh'
    elif "smaller than" in comparator_and_res_part:
        ok = actual_hourly_prod <= expected_res_part
        if not ok.all():
            msg += f'is not inferior to {expected_res_part} MWh'
    elif "equal to" in comparator_and_res_part:
        ok = (actual_hourly_prod - expected_res_part).abs() <= 1e-6
        if not ok.all():
            msg += f'is not close to {expected_res_part} MWh'
    else:
        raise NotImplementedError(f"Unknown comparator '{comparator_and_res_part}'")
    if "zero or" in comparator_and_res_part:
        ok = ok | (actual_hourly_prod == 0)
        msg += " (or null)"
    assert ok.all(), msg
    
@then('in area "{area}", during year {year:d}, for cluster "{cluster}" and reserve "{res}", participation of off units to the reserve is always {comparator_and_res_part} MWh')
def check_off_res_participation_for_specific_year_and_cluster_hourly(context, area, year, res, cluster, comparator_and_res_part):
    cluster_off = cluster + "_off"
    check_res_participation_for_specific_year_and_cluster_hourly(context, area, year, res, cluster_off, comparator_and_res_part)

@then('in area "{area}", during year {year:d}, for cluster "{cluster}" and reserve "{res}", the sum over two hours of reserve participation power is always equal to {expected_res_part} MWh')
def check_res_participation_for_specific_year_and_cluster_hourly_sum(context, area, year, res, cluster, expected_res_part):
    actual_hourly_prod = context.soh.get_hourly_res_part_mwh(area, year, res + "_" + cluster)
    expected_res_part=float(expected_res_part)
    for index in range(0, actual_hourly_prod.size - 1):
        sumOverTwoSteps = actual_hourly_prod[index] + actual_hourly_prod[index+1]
        assert abs(sumOverTwoSteps - expected_res_part) <= 1e-6

@then('in area "{area}", during year {year:d}, for cluster "{cluster}" and reserve "{res}", on "{date}", reserve participation power is {res_part} MWh')
def check_res_participation_for_specific_year_hour_and_cluster(context, area, year, res, cluster, date, res_part):
    actual_res_part = context.soh.get_res_part_for_date_mwh(area, year, date, res + "_" + cluster)
    assert_double_close(float(actual_res_part), float(res_part), 1e-6)
    
@then('in area "{area}", during year {year:d}, for reserve "{res}", reserve unsupplied power is always {comparator_and_unsupplied} MWh')
def check_res_unsp_for_specific_year_hourly(context, area, year, res, comparator_and_unsupplied):
    expected_res_unsupplied = float(comparator_and_unsupplied.split(" ")[-1])
    actual_hourly_res_unsp = context.soh.get_hourly_reserve_unsp_energy(area, year, res)
    msg="At least one value in reserve unsupplied power "
    if "greater than" in comparator_and_unsupplied:
        ok = actual_hourly_res_unsp >= expected_res_unsupplied
        if not ok.all():
            msg += f'is not superior to {expected_res_unsupplied} MWh'
    elif "smaller than" in comparator_and_unsupplied:
        ok = actual_hourly_res_unsp <= expected_res_unsupplied
        if not ok.all():
            msg += f'is not inferior to {expected_res_unsupplied} MWh'
    elif "equal to" in comparator_and_unsupplied:
        ok = abs(actual_hourly_res_unsp - expected_res_unsupplied) <= 1e-6
        if not ok.all():
            msg += f'is not close to {expected_res_unsupplied} MWh'
    else:
        raise NotImplementedError(f"Unknown comparator '{comparator_and_unsupplied}'")
    if "zero or" in comparator_and_unsupplied:
        ok = ok | (actual_hourly_res_unsp == 0)
        msg += " (or null)"
    assert ok.all(), msg

@then('in area "{area}", on "{date}" of year {year:d}, hydro storage {injection_or_pumping_or_level} is of {value_hydro} MWh')
def check_hydro_values_for_specific_year_hour(context, area, year, date, injection_or_pumping_or_level, value_hydro):
    if "injection" in injection_or_pumping_or_level:
        actual_hydro_value = context.soh.get_values_hydro_for_specific_hour_mwh(area, year, date, "H. STOR")
    elif "pumping" in injection_or_pumping_or_level:
        actual_hydro_value = context.soh.get_values_hydro_for_specific_hour_mwh(area, year, date, "H. PUMP")
    elif "level" in injection_or_pumping_or_level:
        actual_hydro_value = context.soh.get_values_hydro_for_specific_hour_mwh(area, year, date, "H. LEV")
    else:
        raise NotImplementedError(f"Unknown value for variable injection_or_pumping_or_level '{injection_or_pumping_or_level}'")
    assert_double_close(float(actual_hydro_value), float(value_hydro), 1e-6)

@then('in area "{area}", on "{date}" of year {year:d}, storage {injection_or_withdrawal} for cluster "{cluster}" is of {value_storage} MW')
def check_storages_values_for_specific_year_hour_and_cluster(context, area, year, date, injection_or_withdrawal, cluster, value_storage):
    if "injection" in injection_or_withdrawal:
        actual_storage_value = context.soh.get_values_for_st_storage_cluster_for_specific_hour_mw(area, year, date, cluster, "P-injection - MW")
    elif "withdrawal" in injection_or_withdrawal:
        actual_storage_value = context.soh.get_values_for_st_storage_cluster_for_specific_hour_mw(area, year, date, cluster, "P-withdrawal - MW")
    else:
        raise NotImplementedError(f"Unknown value for variable injection_or_withdrawal '{injection_or_withdrawal}'")
    assert_double_close(float(actual_storage_value.item()), float(value_storage), 1e-6)
