import pytest
import subprocess
import sys

from pathlib import Path

class StudyReference(object):

    def __init__(self, path, ref_path):
        self.path = Path(path).resolve()
        self.ref_path = Path(ref_path).resolve()

    def run_and_compare(self, exe):
        res = subprocess.run([exe, self.path])
        assert (res.returncode == 0), "The exec failed for study: " + str(self.path)

        out_path = self.path / "input/bindingconstraints/"
        res = subprocess.run(["diff", "-bur", out_path, self.ref_path])
        assert (res.returncode == 0), "Wrong results for study: " + str(self.path)

study_list = []
study_list.append(StudyReference("../resources/Antares_Simulator_Tests/medium-tests/039 Multistage study-4-Kirchhoff", "reference/39"))
study_list.append(StudyReference("../resources/Antares_Simulator_Tests/medium-tests/043 Multistage study-8-Kirchhoff", "reference/43"))
study_list.append(StudyReference("../resources/Antares_Simulator_Tests/long-tests/079 Zero  Power Balance - Type 1", "reference/79"))


@pytest.mark.kirchhoff
def test_kirchhoff_0(exe_kirchhoff_path):
    study_list[0].run_and_compare(exe_kirchhoff_path)

@pytest.mark.kirchhoff
def test_kirchhoff_1(exe_kirchhoff_path):
    study_list[1].run_and_compare(exe_kirchhoff_path)

@pytest.mark.kirchhoff
def test_kirchhoff_2(exe_kirchhoff_path):
    study_list[2].run_and_compare(exe_kirchhoff_path)
