import subprocess
import sys
import os

from pathlib import Path

class StudyList(object):

    def __init__(self, name, path):
        self.name = name
        self.path = Path(path).resolve()

    def run_valgrind_base(self, exe):
        outFile = "--log-file=" + self.name + ".valgrind.out"
        res = subprocess.run(["valgrind", outFile, exe, self.path])
        assert (res.returncode == 0), "The exec failed for study: " + str(self.path)

    def run_valgrind_massif(self, exe):
        outFile = "--massif-out-file=" + self.name + ".valgrind.massif"
        res = subprocess.run(["valgrind" , "--tool=massif", outFile, exe, self.path])
        assert (res.returncode == 0), "The exec failed for study: " + str(self.path)

    def run_time(self, exe):
        outFile = self.name + ".time.txt"
        res = subprocess.run(["/bin/time" , "-o", outFile, "-v", exe, self.path])
        assert (res.returncode == 0), "The exec failed for study: " + str(self.path)

    def run_all(self, exe):
        self.run_valgrind_base(exe)
        self.run_valgrind_massif(exe)
        self.run_time(exe)


study_list = []
study_list.append(StudyList("short", "../resources/Antares_Simulator_Tests/short-tests/001 One node - passive/"))
study_list.append(StudyList("medium", "../resources/Antares_Simulator_Tests/medium-tests/043 Multistage study-8-Kirchhoff"))
study_list.append(StudyList("long", "../resources/Antares_Simulator_Tests/long-tests/079 Zero  Power Balance - Type 1"))


def performance(solver_path):
    for i in range(study_list.length()
        study_list[i].run_all(solver_path)

performance("/home/payetvin/Antares_Simulator/_build_debug/solver/antares-8.6-solver")
