from abc import ABC, abstractmethod
import subprocess
from pathlib import Path
from utils.assertions import check


class StudyRun(ABC):
    def __init__(self, study_path, exe_path, options):
        self.study_path = study_path
        self.exe_path = Path(exe_path).resolve()
        self.options = list(options)
        self.raise_exception_on_failure = True
        self.return_code = 0

    def no_exception_raised_on_failure(self):
        # Tell the run not to raise an exception when run fails
        self.raise_exception_on_failure = False

    def run(self):
        print(f"\nRunning the study with {self.exe_path}")
        command = self._make_command()

        process = subprocess.Popen(command, stdout=subprocess.PIPE, stderr=subprocess.DEVNULL)
        process.communicate()

        self.return_code = process.returncode

        if not self.raise_exception_on_failure:
            return

        check(self.success(), f"Solver returned error {self.return_code}")


    @abstractmethod
    def _make_command(self):
        pass

    def get_return_code(self):
        return self.return_code

    def success(self):
        return self.return_code == 0


class RunStudyWithSolver(StudyRun):
    def _make_command(self):
        return [str(self.exe_path), "-i", str(self.study_path), *self.options]


class RunStudyWithProblemGenerator(StudyRun):
    def _make_command(self):
        return [str(self.exe_path), str(self.study_path), *self.options]
