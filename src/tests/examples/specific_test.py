from pathlib import Path

import sys
import glob
import shutil

import subprocess

import pytest

from study import Study

ALL_STUDIES_PATH = Path('../resources/Antares_Simulator_Tests')

def find_log_path(output_dir):
    op = []
    for path in Path(output_dir).rglob('simulation.log'):
        op.append(path)
    assert len(op) == 1
    return op[0]

def check_log_values(output_path: Path, yearsError, logLevel):
    log_path = find_log_path(output_path)
    log_file = open(str(log_path), 'r')

    # define expected log for each year
    expected_logs = []
    found_list = []
    for year in yearsError.keys():
        weeks = yearsError[year]
        week_label = "week" if len(weeks) == 1 else "weeks"
        expected_week_value = ""
        for week in weeks:
            expected_week_value += " " + str(week)
        expected_logs.append(
            "[solver]["+ logLevel +"] Year " + str(year) + " failed at " + week_label + expected_week_value + ".")
        found_list.append(False)

    # search in all log content and store found result for each year
    for log in log_file:
        for i in range(len(expected_logs)):
            if expected_logs[i] in log:
                found_list[i] = True

    for i in range(len(expected_logs)):
        assert found_list[i], "'" + expected_logs[i] + "'" + " log not found in simulation.log"


def find_mps_path(output_dir, mps_file):
    op = []
    for path in Path(output_dir).rglob(mps_file):
        op.append(path)
    assert len(op) == 1 , "'" + mps_file + "'" + " not found in output directory"
    return op[0]

def check_mps_availability(output_path: Path, yearsError):
    for year in yearsError.keys():
        for week in yearsError[year]:
            find_mps_path(output_path, "problem-"+ str(year) +"-"+str(week)+".mps")

def check_no_mps_available(output_path):
    op = []
    for path in Path(output_path).rglob("*.mps"):
        op.append(path)
    assert len(op) == 0, "mps file found in output directory with dry verbosity option"


def remove_outputs(study_path):
    output_path = study_path / 'output'
    files = glob.glob(str(output_path))
    for f in files:
        shutil.rmtree(f)


def launch_solver(solver_path, study_path, use_ortools=False, ortools_solver="sirius"):
    # Clean study output
    remove_outputs(study_path)

    solver_path_full = str(Path(solver_path).resolve())

    command = [solver_path_full, "-i", str(study_path)]
    if use_ortools:
        command.append('--use-ortools')
        command.append('--ortools-solver=' + ortools_solver)
    process = subprocess.Popen(command, stdout=subprocess.PIPE, stderr=None)
    output = process.communicate()

    return process.returncode


def check_all_unfeasible_options(solver_path, study_path, years_error,years_warning):
    output_path = study_path / 'output'

    st = Study(str(study_path))
    st.check_files_existence()

    # Option error-verbose : mps and error
    st.set_variable(variable="include-unfeasible-problem-behavior", value="error-verbose", file_nick_name="general")
    result_code = launch_solver(solver_path, study_path)
    assert result_code != 0
    check_log_values(output_path, years_error, "fatal")
    check_mps_availability(output_path, years_error)

    # Option error-dry : no mps and error
    st.set_variable(variable="include-unfeasible-problem-behavior", value="error-dry", file_nick_name="general")
    result_code = launch_solver(solver_path, study_path)
    assert result_code != 0
    check_log_values(output_path, years_error, "fatal")
    check_no_mps_available(output_path)

    # Option warning-verbose : mps and warning
    st.set_variable(variable="include-unfeasible-problem-behavior", value="warning-verbose", file_nick_name="general")
    result_code = launch_solver(solver_path, study_path)
    assert result_code == 0
    check_log_values(output_path, years_warning, "warns")
    check_mps_availability(output_path, years_warning)

    # Option warning-dry : no mps and warning
    st.set_variable(variable="include-unfeasible-problem-behavior", value="warning-dry", file_nick_name="general")
    result_code = launch_solver(solver_path, study_path)
    assert result_code == 0
    check_log_values(output_path, years_warning, "warns")
    check_no_mps_available(output_path)

## TESTS ##
@pytest.mark.short
def test_unfeasible_problem_01(solver_path):
    study_path = ALL_STUDIES_PATH / "specific-tests" / "unfeasible-problem" / "unfeasible_problem_01"
    years_warning = {3: [19], 6: [42, 43, 49], 7: [19], 8: [19]}
    years_error = {3: [19]}
    check_all_unfeasible_options(solver_path, study_path,years_error,years_warning)

@pytest.mark.short
def test_unfeasible_problem_02(solver_path):
    study_path = ALL_STUDIES_PATH / "specific-tests" / "unfeasible-problem" / "unfeasible_problem_02"
    years_warning = {6: [22,23,29]}
    years_error = {6: [22]}
    check_all_unfeasible_options(solver_path, study_path, years_error, years_warning)

@pytest.mark.short
def test_unfeasible_problem_03(solver_path):
    study_path = ALL_STUDIES_PATH / "specific-tests" / "unfeasible-problem" / "unfeasible_problem_03"
    years_warning = {3: [19], 6: [42, 43, 49], 7: [19], 8: [19]}
    years_error = {3: [19]}
    check_all_unfeasible_options(solver_path, study_path, years_error, years_warning)

@pytest.mark.short
def test_unfeasible_problem_04(solver_path):
    study_path = ALL_STUDIES_PATH / "specific-tests" / "unfeasible-problem" / "unfeasible_problem_04"
    years_warning = {6: [22, 23, 29]}
    years_error = {6: [22]}
    check_all_unfeasible_options(solver_path, study_path, years_error, years_warning)

@pytest.mark.medium
def test_unfeasible_problem_05(solver_path):
    study_path = ALL_STUDIES_PATH / "specific-tests" / "unfeasible-problem" / "unfeasible_problem_05"
    #Not all years with errors are tested, there is more than 200 years with errors
    years_warning = {10: [19], 11: [19], 16: [19],29: [19],  41: [42, 43, 49],43: [42, 43, 49],46: [42, 43, 49],142: [42, 43, 49]}
    years_error = {10: [19]}
    check_all_unfeasible_options(solver_path, study_path, years_error, years_warning)

@pytest.mark.short
def test_unfeasible_problem_06(solver_path):
    study_path = ALL_STUDIES_PATH / "specific-tests" / "unfeasible-problem" / "unfeasible_problem_06"
    years_warning = {3: [19], 6: [42, 43, 49], 7: [19], 8: [19]}
    years_error = {3: [19]}
    check_all_unfeasible_options(solver_path, study_path, years_error, years_warning)

@pytest.mark.short
def test_unfeasible_problem_07(solver_path):
    study_path = ALL_STUDIES_PATH / "specific-tests" / "unfeasible-problem" / "unfeasible_problem_07"
    years_warning = {1: [1], 3: [52], 5: [1], 7: [52], 9: [52]}
    years_error = {1: [1]}
    check_all_unfeasible_options(solver_path, study_path, years_error, years_warning)

@pytest.mark.short
def test_unfeasible_problem_08(solver_path):
    study_path = ALL_STUDIES_PATH / "specific-tests" / "unfeasible-problem" / "unfeasible_problem_08"
    years_warning = {3: [51],7: [51],9: [51]}
    years_error = {3: [51]}
    check_all_unfeasible_options(solver_path, study_path, years_error, years_warning)

@pytest.mark.short
def test_unfeasible_problem_09(solver_path):
    study_path = ALL_STUDIES_PATH / "specific-tests" / "unfeasible-problem" / "unfeasible_problem_09"
    years_warning = {1: [1], 3: [52], 5: [1], 7: [52], 9: [52]}
    years_error = {1: [1]}
    check_all_unfeasible_options(solver_path, study_path, years_error, years_warning)

@pytest.mark.short
def test_unfeasible_problem_10(solver_path):
    study_path = ALL_STUDIES_PATH / "specific-tests" / "unfeasible-problem" / "unfeasible_problem_10"
    years_warning = {3: [51],7: [51], 9: [51]}
    years_error = {3: [51]}
    check_all_unfeasible_options(solver_path, study_path, years_error, years_warning)
