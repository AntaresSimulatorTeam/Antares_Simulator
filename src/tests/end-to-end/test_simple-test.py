import os
from pathlib import Path
import tempfile as tpf
from distutils.dir_util import copy_tree

import subprocess

import numpy as np

def find_integrity_path(output_dir):
    op = []
    for path in Path(output_dir).rglob('checkIntegrity.txt'):
        op.append(path)
    assert len(op) == 1
    return op[0]

def get_integrity_check_values(output : Path) -> np.array :
    integrity_path = find_integrity_path(output)
    integrity_file = open(str(integrity_path), 'r')
    output_values = list()
    for x in integrity_file:
        output_values.append(float(x))
    assert len(output_values) == 8
    return output_values

def find_solver_path():
    def is_exe(fpath):
        return os.path.isfile(fpath) and os.access(fpath, os.X_OK)

    search_result = list()
    for path in Path.cwd().parent.parent.parent.rglob('solver/antares-*.*-solver*'):
        search_result.append(path)
    # Eliminate swap version
    solver = list(filter(lambda x: "swap" not in str(x) and is_exe(x), search_result))
    assert len(solver) == 1
    return str(solver[0])

def launch_solver(study_path, use_ortools = False, ortools_solver = "sirius"):
    # Find solver executable
    solver_path = find_solver_path()

    command = [solver_path, "-i", str(study_path)]
    if use_ortools:
        command.append('--use-ortools')
        command.append('--ortools-solver='+ortools_solver)
    process = subprocess.Popen(command, stdout=subprocess.PIPE, stderr=None)
    output = process.communicate()

    # TODO check return value
    assert "Solver returned error" not in output[0].decode('utf-8')


def test_free_data_sample_no_ortools(tmp_path):
    # Study copied to temporary directory
    fds = Path.cwd().parent / 'data' / 'free_data_sample'
    copy_tree(str(fds), str(tmp_path))
    study_path = str(tmp_path)
    # Launch antares-solver
    launch_solver(study_path)
    # Check results
    expected_values = np.array([2.85657392370263e+11,
				0.00000000000000e+00,
                                2.85657392370263e+11,
				2.85657392370263e+11,
				2.85657544872729e+11,
				0.00000000000000e+00,
				2.85657544872729e+11,
				2.85657544872729e+11])
    output_path = Path(study_path) / 'output'
    output_values = get_integrity_check_values(output_path)
    np.testing.assert_equal(expected_values, output_values)

def test_free_data_sample_ortools(tmp_path):
    # Study copied to temporary directory
    fds = Path.cwd().parent / 'data' / 'free_data_sample'
    copy_tree(str(fds), str(tmp_path))
    study_path = str(tmp_path)
    # Launch antares-solver
    launch_solver(study_path, True, 'sirius')
    # Check results
    expected_values = np.array([2.85657392370263e+11,
				0.00000000000000e+00,
                                2.85657392370263e+11,
				2.85657392370263e+11,
				2.85657544872729e+11,
				0.00000000000000e+00,
				2.85657544872729e+11,
				2.85657544872729e+11])
    output_path = Path(study_path) / 'output'
    output_values = get_integrity_check_values(output_path)
    np.testing.assert_equal(expected_values, output_values)
