import pytest
import tarfile as tf
from pathlib import Path
import subprocess

class StudyReference(object):

    def __init__(self, solver_path, ref_path, use_ortools, ortools_solver):
        self.solver_path = Path(solver_path).resolve()
        self.ref_path = Path(ref_path).resolve()
        self.use_ortools = use_ortools
        self.ortools_solver = ortools_solver

    def prepare(self):
        with tf.open(self.ref_path) as f:
            f.extractall('.')

    def run_and_compare(self):
        fname_noext = str(self.ref_path.stem)
        command = [self.solver_path, fname_noext]
        if self.use_ortools:
            command.extend(["--use-ortools", f"--ortools-solver {ortools_solver}"])

        res = subprocess.run(command)
        assert (res.returncode == 0), f"The exec failed for study: {fname_noext}"

        ref_files = list(Path(f"{fname_noext}/reference").glob('*.mps'))
        assert ref_files

        output_files = list(Path(f"{fname_noext}/output").glob('**/*.mps'))
        assert output_files

        assert len(output_files) == len(ref_files)

        zipped_list = [(ref, output) for ref in ref_files for output in output_files if ref.name == output.name]
        for pair in zipped_list:
            ref_content = open(pair[0]).read()
            output_content = open(pair[1]).read()
            assert ref_content == output_content, f"Difference between files {pair[0]} and {pair[1]}"

def test_mps_025(solver_path, reference_archive_path, use_ortools, ortools_solver):
    reference = StudyReference(solver_path, reference_archive_path, use_ortools, ortools_solver)
    reference.prepare()
    reference.run_and_compare()
