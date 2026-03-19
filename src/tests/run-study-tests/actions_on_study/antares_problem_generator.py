import subprocess
import os
from pathlib import Path
from utils.assertions import check


class AntaresProblemGeneratorRun:
    def __init__(self, study_path, antares_problem_generator_path):
        self.study_path = study_path
        print(f"antares-problem-generator initialized with study path: {self.study_path}")
        self.api_exe_path = antares_problem_generator_path
        self.raise_exception_on_failure = True
        self.return_code = 0

    def no_exception_raised_on_failure(self):
        # Tell the run not to raise an exception when run fails
        self.raise_exception_on_failure = False

    def run(self):
        print("\nRunning the study")
        antares_problem_generator = str(Path(self.api_exe_path).resolve())
        print(f"Using antares-problem-generator executable at {antares_problem_generator}")
        command = [antares_problem_generator, str(self.study_path)]
        print(f"Running command: {' '.join(command)}")
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
