import pytest
import subprocess
import sys

from pathlib import Path
from utils.find_output import find_dated_output_folder
from parse_studies.look_for_studies import look_for_studies


def run_and_compare(exe, path):
    clusterToGenFile = open(path / "clustersToGen.txt", 'r')
    line = clusterToGenFile.readline().rstrip() # remove new line char
    clusterToGenFile.close()

    res = subprocess.run([exe, "--thermal=" + line, path])
    assert (res.returncode == 0), "The exec failed for study: " + str(path)

    out_path = path / find_dated_output_folder(path) / "ts-generator"
    ref_path = path / "output" / "reference" / "ts-generator"
    res = subprocess.run(["diff", "-bur", out_path, ref_path])
    assert (res.returncode == 0), "Wrong results for study: " + str(path)


ROOT_FOLDER = Path('../resources/batches').resolve()
study_paths = look_for_studies(ROOT_FOLDER)


@pytest.mark.tsgenerator
def test_ts_generator(exe_tsgenerator_path):
    for study in study_paths:
        run_and_compare(exe_tsgenerator_path, study)
