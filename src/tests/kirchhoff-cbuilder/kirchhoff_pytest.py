import pytest
import subprocess
import sys

nb_test = 3

files = [[0 for x in range(2)] for y in range(nb_test)]
files[0][0] = "../resources/Antares_Simulator_Tests/medium-tests/039 Multistage study-4-Kirchhoff"
files[0][1] = "reference/39"
files[1][0] = "../resources/Antares_Simulator_Tests/medium-tests/043 Multistage study-8-Kirchhoff"
files[1][1] = "reference/43"
files[2][0] = "../resources/Antares_Simulator_Tests/long-tests/079 Zero  Power Balance - Type 1"
files[2][1] = "reference/79"


def run_and_compare(path, ref_path, exe):
    res = subprocess.run([exe, path])
    assert (res.returncode == 0), "The exec failed for study: " + path

    out_path = path + "/input/bindingconstraints/"
    res = subprocess.run(["diff", "-bur", out_path, ref_path])
    assert (res.returncode == 0), "Wrong results for study: " + path

@pytest.mark.kirchhoff
def test_kirchhoff_0(exe_kirchhoff_path):
    run_and_compare(files[0][0], files[0][1], exe_kirchhoff_path)

@pytest.mark.kirchhoff
def test_kirchhoff_1(exe_kirchhoff_path):
    run_and_compare(files[1][0], files[1][1], exe_kirchhoff_path)

@pytest.mark.kirchhoff
def test_kirchhoff_2(exe_kirchhoff_path):
    run_and_compare(files[2][0], files[2][1], exe_kirchhoff_path)
