import configparser
import subprocess
import datetime
import json
import sys
import os
import re

from pathlib import Path

# needed to find the execution_info.ini
def get_latest_output_folder(path):
    output_path_study = os.path.join(path, "output")
    files = os.listdir(output_path_study)
    paths = [os.path.join(output_path_study, basename) for basename in files]
    return max(paths, key=os.path.getctime)

# used to get the memory and time from the /bin/time -v file
def search_patern_in_file(file_name, pattern):
    with open(file_name) as f:
        s = f.read()
        m = re.search(pattern, s)
        if m:
            return m.groups(1)[1]
        else:
            raise Exception("Sorry, no numbers below zero")

def get_git_revision_hash() -> str:
    return subprocess.check_output(['git', 'rev-parse', 'HEAD']).decode('ascii').strip()

class StudyList(object):

    def __init__(self, name, path):
        self.name = name                        # study name, used in JSON and out_file
        self.path = Path(path).resolve()        # study path
        self.out_file = name + "-metrics.txt"   # name of the /bin/time -v file

    def run_metrics(self, exe):
        res = subprocess.run(["/bin/time" , "-o", self.out_file, "-v", exe, self.path])
        assert (res.returncode == 0), "The exec failed for study: " + str(self.path)

    def get_memory(self):
        return int(search_patern_in_file(self.out_file, "Maximum(.*): (.*)"))

    def get_time(self):
        user_time = float(search_patern_in_file(self.out_file, "User time(.*): (.*)"))
        syst_time = float(search_patern_in_file(self.out_file, "System time(.*): (.*)"))
        return user_time + syst_time

    def create_json(self):
        memory = self.get_memory() / 1024 # from Kb to Mb
        time = self.get_time()
        return { 'peak_memory_mb' : memory, 'time_s' : time }

    # read the [durations_ms] section from the execution_info.ini file in the study output
    def read_execution_ini(self):
        output_folder = get_latest_output_folder(self.path)
        ini_path = os.path.join(output_folder, "execution_info.ini")

        config = configparser.ConfigParser()
        config.read(ini_path)
        return dict(config.items('durations_ms'))


# list of studies to run, needs a unique name and a path to a valid study
study_list = []
study_list.append(StudyList("short", "../resources/Antares_Simulator_Tests/short-tests/001 One node - passive/"))
study_list.append(StudyList("medium", "../resources/Antares_Simulator_Tests/medium-tests/043 Multistage study-8-Kirchhoff"))


def main(solver_path):
    # init and general data for the JSON
    results = {}
    results["commit-id"] = get_git_revision_short_hash()

    date = datetime.datetime.now()
    results["date"] = date.strftime("%x %X")

    # Execution and results for each study
    for studies in study_list:
        # studies.run_metrics(solver_path)
        results[studies.name] = studies.create_json()
        results[studies.name]['execution_info.ini (time in ms)'] = studies.read_execution_ini()

    # Writing the JSON
    with open("results.json", "w") as f:
        f.write(json.dumps(results))
        print(json.dumps(results))

main("/home/payetvin/Antares_Simulator/_build_debug/solver/antares-8.6-solver")
