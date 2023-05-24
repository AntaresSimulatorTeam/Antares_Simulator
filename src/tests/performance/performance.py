import configparser
import subprocess
import datetime
import json
import glob
import sys
import os

from pathlib import Path

# needed to find the execution_info.ini
def get_latest_output_folder(path):
    output_path_study = os.path.join(path, "output")
    files = os.listdir(output_path_study)
    paths = [os.path.join(output_path_study, basename) for basename in files]
    return max(paths, key=os.path.getctime)

def get_git_revision_short_hash() -> str:
    return subprocess.check_output(['git', 'rev-parse', '--short', 'HEAD']).decode('ascii').strip()

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

        data = {}
        data['peak_memory_mb'] = memory
        data['time_s'] = time

        return data

    def read_execution_ini(self):
        output_folder = get_latest_output_folder(self.path)
        ini_path = os.path.join(output_folder, "execution_info.ini")

        config = configparser.ConfigParser()
        config.read(ini_path)
        return dict(config.items('durations_ms'))


# list for studies to run, needs a unique name and path
study_list = []
study_list.append(StudyList("short", "../resources/Antares_Simulator_Tests/short-tests/001 One node - passive/"))
study_list.append(StudyList("medium", "../resources/Antares_Simulator_Tests/medium-tests/043 Multistage study-8-Kirchhoff"))
# study_list.append(StudyList("long", "../resources/Antares_Simulator_Tests/long-tests/079 Zero  Power Balance - Type 1"))


def performance(solver_path):
    # init and general data for the JSON
    results = {}
    results["commit-id"] = get_git_revision_short_hash()

    date = datetime.datetime.now()
    results["date"] = date.strftime("%x %X")

    # Execution and results for each studies
    for studies in study_list:
        # studies.run_metrics(solver_path)
        studies.read_execution_ini()
        results[studies.name] = studies.create_json()
        results[studies.name]['execution_info.ini'] = studies.read_execution_ini()

    # Writing the JSON
    f = open("results.json", "w")
    f.write(json.dumps(results))
    f.close()

performance("/home/payetvin/Antares_Simulator/_build_debug/solver/antares-8.6-solver")
