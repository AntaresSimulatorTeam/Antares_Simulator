import subprocess
import os
from pathlib import Path
from utils.assertions import check

class study_run:
    def __init__(self, study_path, solver_path, solver_name, named_mps_problems, parallel):
        self.study_path = study_path
        self.solver_path = solver_path
        self.solverName = solver_name
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
        command.append('--solver=' + self.solverName)
        if self.named_mps_problems:
            command.append('--named-mps-problems')
        if self.parallel:
            command.append('--force-parallel=2')
        process = subprocess.Popen(command, stdout=subprocess.PIPE, stderr=subprocess.DEVNULL)
        process.communicate()

        self.return_code = process.returncode

        if not self.raise_exception_on_failure:
            return

        check(self.return_code == 0, "Solver returned error")


    def get_return_code(self):
        return self.return_code

    def success(self):
        return self.return_code == 0
