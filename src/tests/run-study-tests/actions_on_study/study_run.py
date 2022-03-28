import subprocess
from pathlib import Path
from utils.assertions import raise_assertion

class study_run:
    def __init__(self, study_path, solver_path, use_ortools, ortools_solver):
        self.study_path = study_path
        self.solver_path = solver_path
        self.use_ortools = use_ortools
        self.ortools_solver = ortools_solver
        self.raise_exception_on_failure = True
        self.return_code = 0

    def no_exception_raised_at_failure(self):
        # Tell the run not to raise an exception when run fails
        self.raise_exception_on_failure = False

    def run(self):
        print("Running the study")
        solver_full_path = str(Path(self.solver_path).resolve())

        command = [solver_full_path, "-i", str(self.study_path)]
        if self.use_ortools:
            command.append('--use-ortools')
            command.append('--ortools-solver=' + self.ortools_solver)
        process = subprocess.Popen(command, stdout=subprocess.PIPE, stderr=subprocess.DEVNULL)
        output = process.communicate()

        self.return_code = process.returncode

        if not self.raise_exception_on_failure:
            return
        # TODO check return value
        if "Solver returned error" in output[0].decode('utf-8'):
            raise_assertion("Solver returned error")

    def get_return_code(self):
        return self.return_code

    def success(self):
        return self.return_code == 0