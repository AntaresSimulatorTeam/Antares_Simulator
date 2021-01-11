from pathlib import Path
import os
import sys
import glob
import shutil

import numpy as np
import subprocess

from study import study

import pytest


def searching_all_directories(directory):
    dir_path = Path(directory)
    assert(dir_path.is_dir())
    dir_list = []
    for x in dir_path.iterdir():
        if x.is_dir():
            dir_list.append(x)
    return dir_list

def find_output_result_dir(output_dir):
    list_output_dir = searching_all_directories(output_dir)
    assert len(list_output_dir) == 1

    list_dir = searching_all_directories(list_output_dir[0])

    dir_list = []
    for x in list_dir:
        dir_path = Path(x)
        if dir_path.is_dir() and (dir_path.name == "adequacy" or dir_path.name == "economy" or dir_path.name == "adequacy-draft"):
            dir_list.append(x)
    assert len(dir_list) == 1
    return dir_list[0]

def get_output_values(values_path : Path) -> np.array :

    max_row = 8760
    # skip_header=7 in order to skip the header of the file
    # usecols=range(5, 25) in order to select the columns with the 20 group of production
    # max_rows=8760 to select all year
    output_values = np.genfromtxt(values_path, delimiter='\t', skip_header=7, max_rows=max_row)
    return output_values

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
    process = subprocess.Popen(command, stdout=subprocess.PIPE, stderr=None)
    output = process.communicate()

    # TODO check return value
    assert "Solver returned error" not in output[0].decode('utf-8')

def generate_reference_values(solver_path, path, use_ortools, ortools_solver):

    enable_study_output(path,True)

    reference_path = path / 'reference'
    os.makedirs(reference_path, exist_ok=True)
    launch_solver(solver_path,path, use_ortools, ortools_solver)

    output_path = path / 'output'

    result_dir = find_output_result_dir(output_path)
    shutil.copytree(result_dir, reference_path / 'output' / result_dir.name)

def run_study(solver_path, path, use_ortools, ortools_solver):
    # Launch antares-solver
    launch_solver(solver_path, path, use_ortools, ortools_solver)
    
def enable_study_output(study_path, enable):
    st = study(str(study_path))
    st.check_files_existence()

    synthesis_value = "true" if enable else "false"
    st.set_variable(variable = "synthesis", value = synthesis_value, file_nick_name="general")

def compare_directory(result_dir, reference_dir):
    assert (result_dir.is_dir())
    assert (reference_dir.is_dir())
    dir_list = []
    for x in result_dir.iterdir():
        if x.is_dir():
            if x.name != 'grid':
                compare_directory(x, reference_dir / x.name)
        else:

            if x.name != 'id-daily.txt' :
                reference_values = get_output_values(reference_dir / x.name)
                output_values = get_output_values(x)

                np.testing.assert_allclose(reference_values, output_values, rtol=1e-4, atol=0,equal_nan=True, err_msg="values dismatch in " + str(reference_dir / x.name), verbose=True)

def check_output_values(path):
    result_dir = find_output_result_dir(path / 'output')
    reference_dir = find_output_result_dir(path / 'reference')
    compare_directory(result_dir, reference_dir)

## TESTS ##
@pytest.mark.short
def test_output_compare(use_ortools, ortools_solver, solver_path, study_path):
    path = Path(study_path)
    enable_study_output(path, True)
    run_study(solver_path, path, use_ortools, ortools_solver)
    check_output_values(path)



