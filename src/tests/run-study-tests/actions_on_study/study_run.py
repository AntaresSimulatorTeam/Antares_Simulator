import subprocess
import os
from pathlib import Path
from utils.assertions import check

class study_run:
    def __init__(self, study_path, solver_path, use_ortools, ortools_solver, named_mps_problems, parallel):
        self.study_path = study_path
        self.solver_path = solver_path
        self.use_ortools = use_ortools
        self.ortools_solver = ortools_solver
        self.named_mps_problems = named_mps_problems
        self.parallel = parallel
        self.raise_exception_on_failure = True
        self.return_code = 0

    def no_exception_raised_on_failure(self):
        # Tell the run not to raise an exception when run fails
        self.raise_exception_on_failure = False

    def run(self):
        print("\nRunning the study")
        solver_full_path = str(Path(self.solver_path).resolve())

        command = [solver_full_path, "-i", str(self.study_path)]
        self.use_ortools = True
        self.ortools_solver = "sirius"
        if self.use_ortools:
            command.append('--use-ortools')
            command.append('--ortools-solver=' + self.ortools_solver)
        if self.named_mps_problems:
            command.append('--named-mps-problems')
        if self.parallel:
            command.append('--force-parallel=4')

        process = subprocess.Popen(command, stdout=subprocess.PIPE, stderr=subprocess.DEVNULL)
        process.communicate()

        self.return_code = process.returncode

        if not self.raise_exception_on_failure:
            return

        check(self.success(), f"Solver returned error {self.return_code}")


    def get_return_code(self):
        return self.return_code

    def success(self):
        return self.return_code == 0
