import subprocess
import json
import sys
import os

from pathlib import Path

class StudyList(object):

    def __init__(self, name, path):
        self.name = name
        self.path = Path(path).resolve()
        self.outFile = name + "-metrics.txt"

    def run_metrics(self, exe):
        res = subprocess.run(["/bin/time" , "-o", self.outFile, "-v", exe, self.path])
        assert (res.returncode == 0), "The exec failed for study: " + str(self.path)

    def get_memory(self):
        ps = subprocess.run(["sed -n -e 's/^.*Maximum.*: //p' " + self.outFile], shell=True, capture_output=True)
        return int(ps.stdout)

    def get_time(self):
        ps = subprocess.run(["sed -n -e 's/^.*User time.*: //p' " + self.outFile], shell=True, capture_output=True)
        ps2 = subprocess.run(["sed -n -e 's/^.*System time.*: //p' " + self.outFile], shell=True, capture_output=True)
        return float(ps.stdout) + float(ps2.stdout)

    def create_json(self):
        memory = self.get_memory() / 1024 # from Kb to Mb
        time = self.get_time()

        data_memory = {}
        data_memory['name'] = "Memory " + self.name + " study"
        data_memory['value'] = memory
        data_memory['unit'] = "Mb"

        data_time = {}
        data_time['name'] = "Execution time " + self.name + " study"
        data_time['value'] = time
        data_time['unit'] = "seconds"
        data = [data_time, data_memory]
        json_data = json.dumps(data)

        print(json_data)



study_list = []
study_list.append(StudyList("short", "../resources/Antares_Simulator_Tests/short-tests/001 One node - passive/"))
# study_list.append(StudyList("medium", "../resources/Antares_Simulator_Tests/medium-tests/043 Multistage study-8-Kirchhoff"))
# study_list.append(StudyList("long", "../resources/Antares_Simulator_Tests/long-tests/079 Zero  Power Balance - Type 1"))


def performance(solver_path):
    for studies in study_list:
        studies.run_metrics(solver_path)
        studies.create_json()

performance("/home/payetvin/Antares_Simulator/_build_debug/solver/antares-8.6-solver")
