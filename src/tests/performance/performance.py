import subprocess
import sys
import os

from pathlib import Path

class StudyList(object):

    def __init__(self, path):
        self.path = Path(path).resolve()

    def run_valgrind_base(self, exe):
        valgrindOutFile = "--log-file=valgrind.out"
        res = subprocess.run(["valgrind", valgrindOutFile, exe, self.path])
        assert (res.returncode == 0), "The exec failed for study: " + str(self.path)

    def run_valgrind_massif(self, exe):
        massifOutFile = "valgrind.massif"
        res = subprocess.run(["valgrind" , "--tool=massif", "--massif-out-file=" + massifOutFile, exe, self.path])
        assert (res.returncode == 0), "The exec failed for study: " + str(self.path)

    def run_time(self, exe):
        massifOutFile = "time.txt"
        res = subprocess.run(["/bin/time" , "-o", timeOutFile, "-v", exe, self.path])
        assert (res.returncode == 0), "The exec failed for study: " + str(self.path)

    def run_all(self, exe):
        self.run_valgrind_base(exe)
        self.run_valgrind_massif(exe)
        self.run_time(exe)


study_list = []
study_list.append(StudyList("../resources/Antares_Simulator_Tests/short-tests/001 One node - passive/"))
study_list.append(StudyList("../resources/Antares_Simulator_Tests/medium-tests/043 Multistage study-8-Kirchhoff"))
study_list.append(StudyList("../resources/Antares_Simulator_Tests/long-tests/079 Zero  Power Balance - Type 1"))

def valgrind_short(solver_path):
    study_list[0].run_all(solver_path)

def valgrind_medium(solver_path):
    study_list[1].run_all(solver_path)

def valgrind_long(solver_path):
    study_list[2].run_all(solver_path)

def performance(solver_path):
    valgrind_short(solver_path)
    valgrind_medium(solver_path)
    valgrind_long(solver_path)

performance("/home/payetvin/Antares_Simulator/_build_debug/solver/antares-8.6-solver")
