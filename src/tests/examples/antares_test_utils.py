from pathlib import Path
from os import walk
from os.path import basename, sep
import sys
import glob
import shutil

import pandas as pd
import subprocess
import abc

from study import Study

import pytest
from trim_column_name import trim_digit_after_last_dot
from read_utils import read_csv

ALL_STUDIES_PATH = Path('../resources/Antares_Simulator_Tests').resolve()

def find_simulation_folder(output_dir):
    for root, dirs, files in walk(output_dir):
        if basename(root) in ["adequacy", "economy", "adequacy-draft"]:
            return Path(root)

def get_headers(df) -> set :
    return set(df.columns)

def remove_outputs(study_path):
    output_path = study_path / 'output'
    files = glob.glob(str(output_path))
    for f in files:
        shutil.rmtree(f)

def launch_solver(solver_path, study_path, use_ortools = False, ortools_solver = "sirius"):
    # Clean study output
    remove_outputs(study_path)

    solver_path_full = str(Path(solver_path).resolve())

    command = [solver_path_full, "-i", str(study_path)]
    if use_ortools:
        command.append('--use-ortools')
        command.append('--ortools-solver='+ortools_solver)
    process = subprocess.Popen(command, stdout=subprocess.PIPE, stderr=subprocess.DEVNULL)
    output = process.communicate()

    # TODO check return value
    assert "Solver returned error" not in output[0].decode('utf-8')

def run_study(solver_path, path, use_ortools, ortools_solver):
    # Launch antares-solver
    launch_solver(solver_path, path, use_ortools, ortools_solver)

def enable_study_output(study_path, enable):
    st = Study(str(study_path))
    st.check_files_existence()

    synthesis_value = "true" if enable else "false"
    st.set_variable(variable = "synthesis", value = synthesis_value, file_nick_name="general")

def skip_folder(folder):
    if basename(folder) == 'grid':
        return True
    return False

def skip_file(file):
    if file in ['id-daily.txt', 'id-hourly.txt']:
        return True
    return False

def find_simulation_files(reference_folder, other_folder):
    list_files_to_compare = []

    # Walk through (or traverse) the reference folder
    for current_folder, sub_folders, files in walk(reference_folder):
        if skip_folder(current_folder):
            continue

        for file in files:
            if skip_file(file):
                continue
            current_file_path = Path(current_folder) / file
            list_files_to_compare.append((current_file_path, other_folder / current_file_path.relative_to(reference_folder)))

    return list_files_to_compare

def print_comparison_report(ref_data_frame, other_data_frame, file_path, col_name):
    # Max number of rows to be printed in the report
    pd.set_option("display.max_rows", 10)

    # At this stage, report contains only 2 comulms with lines that differ
    comparison_report = ref_data_frame[col_name].compare(other_data_frame[col_name])

    # Renaming columns
    comparison_report = comparison_report.rename(columns={'self': 'reference', 'other': 'results'})

    # Print title of the comparison report
    print("\n%s : %s" % (file_path, col_name))

    # Add a column that contains the differences
    comparison_report['diff'] = comparison_report['reference'] - comparison_report['results']
    
    print(comparison_report)

def compare_simulation_files(simulation_files, tol):
    REF_INDEX = 0
    OTHER_INDEX = 1
    at_least_one_diff = False
    for file_pair in simulation_files:
        # Read reference and simulation (other) files
        ref_data_frame = read_csv(file_pair[REF_INDEX])
        other_data_frame = read_csv(file_pair[OTHER_INDEX])

        # Check that reference column titles are a subset of the simulation titles
        ref_column_titles = get_headers(ref_data_frame)
        other_column_titles = get_headers(other_data_frame)
        assert ref_column_titles.issubset(other_column_titles),\
               f"The following column(s) is missing in the reference {ref_column_titles.difference(other_column_titles)}"

        for col_name in ref_column_titles:
            try:
                pd.testing.assert_series_equal(ref_data_frame[col_name], other_data_frame[col_name], \
                                               atol=tol.absolute(col_name), rtol=tol.relative(col_name))
            except AssertionError:  # Catch and re-raise exception to print col_name & tolerances
                # Reduced path of files we're comparing
                file_path = sep.join(file_pair[REF_INDEX].absolute().parts[-4:])

                print_comparison_report(ref_data_frame, other_data_frame, file_path, col_name)
                at_least_one_diff = True

    return not at_least_one_diff

class Tolerances(metaclass=abc.ABCMeta):
    default_abs_tol = 0
    default_rel_tol = 1e-4

    @abc.abstractmethod
    def absolute(self, col_name):
        pass
    @abc.abstractmethod
    def relative(self, col_name):
        pass

class Linux_tolerances(Tolerances):
    RTOL_LINUX = {"CO2 EMIS.": 1e-3, "FLOW LIN.": 1e-3, "UCAP LIN.": 1e-3, "H. INFL": 1e-3, "H. STOR": 1e-3,
                           "H. OVFL": 1e-3, "OV. COST": 1e-3, "LIGNITE": 1e-3, "CONG. FEE (ABS.)": 1e-3, "sb": 1e-3,
                           "MARG. COST": 1e-3, "DTG MRG": 1e-3, "BALANCE": 1e-3, "BASE": 1e-3, "MRG. PRICE": 1e-3,
                           "OP. COST": 1e-3, "SEMI BASE": 1e-3, "COAL": 1e-3, "MAX MRG": 1e-3, "UNSP. ENRG": 1e-3}
    ATOL_LINUX = {"CO2 EMIS.": 1, "CONG. FEE (ALG.)": 1, "FLOW LIN.": 1, "UCAP LIN.": 1, "peak": 1, "PEAK": 1,
                           "H. INFL": 1, "H. STOR": 1, "HURDLE COST": 1, "H. OVFL": 1, "LOAD": 1, "CONG. FEE (ABS.)": 1,
                           "sb": 1, "MISC. DTG": 1, "DTG MRG": 1, "BALANCE": 1, "BASE": 1, "OP. COST": 1,
                           "SEMI BASE": 1, "COAL": 1, "p": 1, "MAX MRG": 1, "UNSP. ENRG": 1, "SOLAR": 1, "b": 1,
                           "NODU": 1, "H. ROR": 1}
    def absolute(self, col_name):
        trimmed_col_name = trim_digit_after_last_dot(col_name)
        if trimmed_col_name in self.ATOL_LINUX:
            return self.ATOL_LINUX[trimmed_col_name]
        return self.default_abs_tol

    def relative(self, col_name):
        trimmed_col_name = trim_digit_after_last_dot(col_name)
        if trimmed_col_name in self.RTOL_LINUX:
            return self.RTOL_LINUX[trimmed_col_name]
        return self.default_rel_tol

class Win_tolerances(Tolerances):
    RTOL_WIN = {}
    ATOL_WIN = {}
    def absolute(self, col_name):
        return self.default_abs_tol

    def relative(self, col_name):
        return self.default_rel_tol


def get_tolerances():
    if sys.platform == "linux":
        return Linux_tolerances()
    elif sys.platform == "win32":
        return Win_tolerances()
    else:
        print("Unknown OS")
        raise AssertionError()

def check_output_values(study_path):
    reference_folder = find_simulation_folder(study_path / 'reference')
    other_folder = find_simulation_folder(study_path / 'output')
    simulation_files = find_simulation_files(reference_folder, other_folder)
    tolerances = get_tolerances()
    if not compare_simulation_files(simulation_files, tolerances):
        raise AssertionError()
    remove_outputs(study_path)

