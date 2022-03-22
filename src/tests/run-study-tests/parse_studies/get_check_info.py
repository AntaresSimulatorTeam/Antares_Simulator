import json
import os.path
from pathlib import Path

class checkReader:
    def __init__(self, study_path):
        self.study_path = study_path
        self.check_data_file = study_path / "check-config.json"
        self.checks_for_each_simulation = []

    def data_exists(self):
        return os.path.isfile(self.check_data_file)

    def read(self):
        with open(self.check_data_file) as json_file:
            self.checks_for_each_simulation = json.load(json_file)

    def get_checks(self):
        to_return = []
        for check_set in self.checks_for_each_simulation:
            to_return.append((self.study_path, check_set))
        return to_return


def get_studies_check_info(study_path_list):
    study_check_info = []
    for study_path in study_path_list:
        check_reader = checkReader(study_path)
        if check_reader.data_exists():
            check_reader.read()
            study_check_info.extend(check_reader.get_checks())

    return study_check_info


# ================
# For validation
# ================
ROOT_FOLDER = Path('../../resources/study-collection-sample').resolve()

# study_path_list = [ROOT_FOLDER / "short-tests" / "001 One node - passive"]
study_path_list = [ROOT_FOLDER / "short-tests" / "playlist-psp-misc-ndg"]

studies_check_info = get_studies_check_info(study_path_list)
print("study_path_list :")
print(studies_check_info)


