import pytest
import subprocess
import sys

from pathlib import Path
from utils.find_output import find_dated_output_folder

class StudyReference(object):

    def __init__(self, path):
        self.path = Path(path).resolve()

    def run_and_compare(self, exe):
        clusterToGenFile = open(self.path / "clustersToGen.txt", 'r')
        line = clusterToGenFile.readline().rstrip() # remove new line char
        clusterToGenFile.close()

        res = subprocess.run([exe, "--thermal=" + line, self.path])
        assert (res.returncode == 0), "The exec failed for study: " + str(self.path)

        out_path = self.path / find_dated_output_folder(self.path) / "ts-generator"
        ref_path = self.path / "output" / "reference" / "ts-generator"
        res = subprocess.run(["diff", "-bur", out_path, ref_path])
        assert (res.returncode == 0), "Wrong results for study: " + str(self.path)


study_list = []
study_list.append(StudyReference("../resources/batches/valid-tsgenerator/007 Thermal fleet - Fast unit commitment"))
study_list.append(StudyReference("../resources/batches/valid-tsgenerator/079 Zero  Power Balance - Type 1"))

@pytest.mark.tsgenerator
def test_ts_generator(exe_tsgenerator_path):
    for study in study_list:
        study.run_and_compare(exe_tsgenerator_path)
