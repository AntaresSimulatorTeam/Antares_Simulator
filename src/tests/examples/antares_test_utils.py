from pathlib import Path
import os
import sys
import glob
import shutil


import numpy as np

import subprocess

from study import Study


RTOL_OVERRIDE_LINUX = {"CO2 EMIS." : 1e-3, "FLOW LIN." : 1e-3 , "UCAP LIN." : 1e-3, "H. INFL" : 1e-3 , "H. STOR" : 1e-3 , "H. OVFL" : 1e-3 , "OV. COST" : 1e-3 , "LIGNITE" : 1e-3 , "CONG. FEE (ABS.)" : 1e-3 , "sb" : 1e-3 , "MARG. COST" : 1e-3 , "DTG MRG" : 1e-3 , "BALANCE" : 1e-3 , "BASE" : 1e-3 , "MRG. PRICE" : 1e-3 , "OP. COST" : 1e-3 , "SEMI BASE" : 1e-3 ,"COAL" : 1e-3 , "MAX MRG" : 1e-3 , "UNSP. ENRG" : 1e-3}
ATOL_OVERRIDE_LINUX = {"CO2 EMIS." : 1, "CONG. FEE (ALG.)" : 1, "FLOW LIN." : 1, "UCAP LIN." : 1, "peak" : 1, "PEAK" : 1, "H. INFL" : 1, "H. STOR" : 1, "HURDLE COST" : 1, "H. OVFL" : 1 , "LOAD" : 1, "CONG. FEE (ABS.)" : 1 , "sb" : 1 , "MISC. DTG" : 1 , "DTG MRG" : 1 , "BALANCE" : 1 , "BASE" : 1 , "OP. COST" : 1 , "SEMI BASE" : 1 , "COAL" : 1 , "p" : 1 , "MAX MRG" : 1 , "UNSP. ENRG" : 1 , "SOLAR" : 1 , "b" : 1 , "NODU" : 1 , "H. ROR" : 1}

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

def get_header_values(values_path : Path) -> np.array :
    max_row = 3
    # skip_header=4 in order to skip the unused part header of the file
    # max_rows=3 to select 3 headers row (name, unit, type : std, min, max, exp,...)
    output_values = np.genfromtxt(values_path, delimiter='\t', skip_header=4, max_rows=max_row, dtype=str)
    return output_values

def get_output_values(values_path : Path) -> np.array :
    max_row = 8760
    # skip_header=7 in order to skip the header of the file
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
    st = Study(str(study_path))
    st.check_files_existence()

    synthesis_value = "true" if enable else "false"
    st.set_variable(variable = "synthesis", value = synthesis_value, file_nick_name="general")

def compare_directory(result_dir, reference_dir):
    assert (result_dir.is_dir())
    assert (reference_dir.is_dir())

    for x in result_dir.iterdir():
        if x.is_dir():
            if x.name != 'grid':
                compare_directory(x, reference_dir / x.name)
        else:

            uncompared_file_name = ['id-daily.txt', 'id-hourly.txt']

            if not x.name in uncompared_file_name:
                reference_headers = get_header_values(reference_dir / x.name)
                reference_values = get_output_values(reference_dir / x.name)

                output_headers = get_header_values(x)
                output_values = get_output_values(x)

                np.testing.assert_equal(reference_headers,output_headers, err_msg="headers dismatch in " + str(reference_dir / x.name), verbose=True)


                for i in range(len(output_headers[0])):
                    col_name=output_headers[0,i]
                    err_msg = "values dismatch in '" + str(reference_dir / x.name) + "' for '" + col_name + "' column"
                    rtol=1e-4
                    atol=0

                    if sys.platform=="linux":
                        if col_name in RTOL_OVERRIDE_LINUX:
                            rtol = RTOL_OVERRIDE_LINUX[col_name]
                        if col_name in ATOL_OVERRIDE_LINUX:
                            atol = ATOL_OVERRIDE_LINUX[col_name]

                    if reference_values.ndim > 1:
                        np.testing.assert_allclose(reference_values[:, i], output_values[:, i], rtol=rtol, atol=atol, equal_nan=True, err_msg=err_msg, verbose=True)
                    else:
                        np.testing.assert_allclose(reference_values[i], output_values[i], rtol=rtol, atol=atol, equal_nan=True, err_msg=err_msg, verbose=True)


def check_output_values(study_path):
    result_dir = find_output_result_dir(study_path / 'output')
    reference_dir = find_output_result_dir(study_path / 'reference')
    compare_directory(result_dir, reference_dir)
    remove_outputs(study_path)
