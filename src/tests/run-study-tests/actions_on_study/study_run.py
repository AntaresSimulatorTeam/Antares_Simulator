import subprocess
from pathlib import Path
from utils.assertions import raise_assertion

class study_run:
    def __init__(self, study_path, solver_path, use_ortools, ortools_solver):
        self.study_path = study_path
        self.solver_path = solver_path
        self.use_ortools = use_ortools
        self.ortools_solver = ortools_solver

    def run(self):
        print("Running the study")
        solver_full_path = str(Path(self.solver_path).resolve())

        command = [solver_full_path, "-i", str(self.study_path)]
        if self.use_ortools:
            command.append('--use-ortools')
            command.append('--ortools-solver=' + self.ortools_solver)
        process = subprocess.Popen(command, stdout=subprocess.PIPE, stderr=subprocess.DEVNULL)
        output = process.communicate()

        # TODO check return value
        if "Solver returned error" in output[0].decode('utf-8'):
            raise_assertion("Solver returned error")