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
import csv
import io
from typing import Optional

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
    def _parse_csv_lines(lines):
        if lines is None:
            return None
        reader = csv.DictReader(io.StringIO("".join(lines)))
        return list(reader)

    def _key_of_row(row: dict):
        # Stable key for identifying a row
        keys = [
            "block",
            "component",
            "output",
            "absolute_time_index",
            "block_time_index",
            "scenario_index",
        ]
        if all(k in row for k in keys):
            return tuple((row[k] or "").strip() for k in keys)
        # Fallback: all values ordered by column name
        colnames = sorted(k for k in row.keys() if k is not None)
        return ("__all__", tuple((k, (row[k] or "").strip()) for k in colnames))

    def _index_rows(rows):
        index = {}
        for r in rows:
            index.setdefault(_key_of_row(r), []).append(r)
        return index

    def _as_number(s: str):
        try:
            return float(s)
        except Exception:
            return None

    def _compare_rows(r_ref: dict, r_out: dict):
        diffs = {}
        # Compare common columns; also report missing columns
        ref_cols = set(r_ref.keys())
        out_cols = set(r_out.keys())
        common = ref_cols & out_cols
        for c in sorted(common):
            a_raw = r_ref[c] if r_ref[c] is not None else ""
            b_raw = r_out[c] if r_out[c] is not None else ""
            a = a_raw.strip()
            b = b_raw.strip()
            if a == b:
                continue
            a_num = _as_number(a)
            b_num = _as_number(b)
            if a_num is not None and b_num is not None:
                # Numeric compare to avoid false positives like '1' vs '1.0'
                if a_num != b_num:
                    diffs[c] = (a_raw, b_raw)
            else:
                diffs[c] = (a_raw, b_raw)
        missing_in_out = sorted(ref_cols - out_cols)
        missing_in_ref = sorted(out_cols - ref_cols)
        if missing_in_out:
            diffs["__missing_in_out__"] = missing_in_out
        if missing_in_ref:
            diffs["__missing_in_ref__"] = missing_in_ref
        return diffs

    def _format_key(key_tuple):
        # Format a readable key
        labels = [
            "block",
            "component",
            "output",
            "absolute_time_index",
            "block_time_index",
            "scenario_index",
        ]
        if key_tuple and key_tuple[0] == "__all__":
            return "(raw-row-key from all columns)"
        parts = []
        for i, v in enumerate(key_tuple):
            if i < len(labels):
                parts.append(f"{labels[i]}={v}")
            else:
                parts.append(str(v))
        return ", ".join(parts)

    def _diff_message(name: str, ref_lines, out_lines) -> Optional[str]:
        ref_rows = _parse_csv_lines(ref_lines)
        out_rows = _parse_csv_lines(out_lines)
        if ref_rows is None and out_rows is None:
            return None
        if ref_rows is None:
            return f"{name}: reference is missing but output exists"
        if out_rows is None:
            return f"{name}: output is missing but reference exists"

        ref_index = _index_rows(ref_rows)
        out_index = _index_rows(out_rows)

        ref_keys = set(ref_index.keys())
        out_keys = set(out_index.keys())

        missing_keys = sorted(ref_keys - out_keys)
        extra_keys = sorted(out_keys - ref_keys)

        msg_lines = []
        if missing_keys:
            msg_lines.append(f"- Missing rows in output (samples):")
            for k in missing_keys[:10]:
                msg_lines.append(f"  * {_format_key(k)}")
            if len(missing_keys) > 10:
                msg_lines.append(f"  ... and {len(missing_keys) - 10} more")
        if extra_keys:
            msg_lines.append(f"- Unexpected extra rows (samples):")
            for k in extra_keys[:10]:
                msg_lines.append(f"  * {_format_key(k)}")
            if len(extra_keys) > 10:
                msg_lines.append(f"  ... and {len(extra_keys) - 10} more")

        # Compare lines present in both
        mismatches_shown = 0
        mismatch_limit = 50
        for k in sorted(ref_keys & out_keys):
            ref_list = ref_index[k]
            out_list = out_index[k]
            # If multiple, compare in appearance order
            count = min(len(ref_list), len(out_list))
            for i in range(count):
                diffs = _compare_rows(ref_list[i], out_list[i])
                if diffs:
                    if mismatches_shown == 0:
                        msg_lines.append("- Different values (samples):")
                    mismatches_shown += 1
                    if mismatches_shown <= mismatch_limit:
                        msg_lines.append(f"  * {_format_key(k)}:")
                        for col, pair in diffs.items():
                            if col == "__missing_in_out__":
                                msg_lines.append(f"      columns missing in output: {pair}")
                            elif col == "__missing_in_ref__":
                                msg_lines.append(f"      columns missing in reference: {pair}")
                            else:
                                msg_lines.append(f"      {col}: ref='{pair[0]}' vs out='{pair[1]}'")
            # If different counts for the same key, report
            if len(ref_list) != len(out_list):
                msg_lines.append(
                    f"  * {_format_key(k)}: occurrence count ref={len(ref_list)} vs out={len(out_list)}")

            if mismatches_shown >= mismatch_limit:
                msg_lines.append("  ... display limit reached")
                break

        if msg_lines:
            header = f"{name} does not match reference"
            return header + "\n" + "\n".join(msg_lines)
        return None

    # Optimization 1
    msg1 = _diff_message(
        name="Simulation table (optimization 1)",
        ref_lines=context.sih.get_optim1_simulation_table(),
        out_lines=context.soh.get_optim1_simulation_table(),
    )
    assert msg1 is None, msg1

    # Optimization 2 (if any)
    ref_simulation_table2 = context.sih.get_optim2_simulation_table()
    if ref_simulation_table2:
        msg2 = _diff_message(
            name="Simulation table (optimization 2)",
            ref_lines=ref_simulation_table2,
            out_lines=context.soh.get_optim2_simulation_table(),
        )
        assert msg2 is None, msg2


def should_check(row, key):
    return key in row.headings and len(row[key]) > 0


def run_simulation(context):
    command = build_antares_solver_command(context)
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


@step('the message "{log}" is reported in the logs')
def ckeck_log_exists(context, log):
    for log_line in context.logs_err.splitlines():
        if log in log_line:
            return
    raise AssertionError(f"Log '{log}' is not reported in the logs")


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
