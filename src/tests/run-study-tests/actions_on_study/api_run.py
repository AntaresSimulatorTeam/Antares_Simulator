import subprocess
import os
from pathlib import Path
from utils.assertions import check

class api_run:
    def __init__(self, study_path, api_exe_path, write_mps, output_dir):
        self.study_path = study_path
        print(f"API run initialized with study path: {self.study_path}")
        self.api_exe_path = api_exe_path
        self.write_mps = write_mps
        self.output_dir = str(Path(self.study_path) /"output"/ output_dir)
        self.raise_exception_on_failure = True
        self.return_code = 0

    def no_exception_raised_on_failure(self):
        # Tell the run not to raise an exception when run fails
        self.raise_exception_on_failure = False

    def run(self):
        print("\nRunning the study")
        api_exe_full_path = str(Path(self.api_exe_path).resolve())
        print(f"Using API executable at {api_exe_full_path}")
        command = [api_exe_full_path, "-i", str(self.study_path)]
        if self.write_mps:
            command.append('--write-mps')
        if self.output_dir:
            command.extend(['--output', self.output_dir])
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
