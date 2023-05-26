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

    def create_json(self):
        return { 'peak_memory_mb' : self.get_memory() / 1024 } # convert to Mb

    # read the [sections] from the execution_info.ini file in the study output
    def read_execution_ini(self, json_data):
        output_folder = get_latest_output_folder(self.path)
        ini_path = os.path.join(output_folder, "execution_info.ini")

        config = configparser.ConfigParser()
        config.read(ini_path)
        json_data["durations_s"] = dict(config.items('durations_ms'))
        json_data["optimization problem"] = dict(config.items('optimization problem'))
        json_data["study"] = dict(config.items('study'))

        # convert ms into seconds
        durations = json_data['durations_s']
        for i in durations:
            durations[i] = float(durations[i]) / 1000


# load the JSON list containing studies to benchmark
def read_study_list():
    study_list = []
    with open("studiesToBenchmark.json", "r") as f:
        studies = json.load(f)
        for name in studies:
            study_list.append(StudyList(name, studies[name]))
    return study_list


def main(solver_path):
    # init and general data for the JSON
    results = {}
    results["commit-id"] = get_git_revision_hash()

    date = datetime.datetime.now()
    results["date"] = date.strftime("%x %X")

    # Execution and results for each study
    for studies in read_study_list():
        studies.run_metrics(solver_path)
        results[studies.name] = studies.create_json()
        studies.read_execution_ini(results[studies.name])

    # Writing the JSON
    with open("results.json", "w") as f:
        f.write(json.dumps(results))


solver_path = sys.argv[1]
if not os.path.isfile(solver_path):
    raise Exception("Invalid solver path, file not found")

main(solver_path)
