import pytest
import subprocess
import sys

from pathlib import Path
from utils.find_output import find_dated_output_folder

class StudyReference(object):

    def __init__(self, path, args):
        self.path = Path(path).resolve()

    def run_and_compare(self, exe):
        res = subprocess.run([exe, self.path])
        assert (res.returncode == 0), "The exec failed for study: " + str(self.path)

        out_path = self.path / "ts-generator"
        res = subprocess.run(["diff", "-bur", out_path, self.ref_path])
        assert (res.returncode == 0), "Wrong results for study: " + str(self.path)

@pytest.mark.tsgenerator
def test1():
    assert (1 == 1), "test"
