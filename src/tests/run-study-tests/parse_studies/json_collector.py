import json
import os.path

class checksJsonReader:
    def __init__(self, study_path):
        self.study_path = study_path
        self.check_data_file = study_path / "check-config.json"

        self.test_pairs = []
        self.test_ids = []

    def json_file_exists(self):
        return os.path.isfile(self.check_data_file)

    def read(self):
        with open(self.check_data_file) as json_file:
            test_collection = json.load(json_file)

        for test in test_collection:
            self.test_ids.append(test["name"])
            self.test_pairs.append((self.study_path, test["checks"]))

    def get_ids(self):
        return self.test_ids
    def get_checks(self):
        return self.test_pairs



class jsonCollector:
    def __init__(self, study_paths):
        self.study_paths_ = study_paths
        self.pairs_ = []
        self.test_ids_ = []

    def collect(self):
        for study_path in self.study_paths_:
            checks_json_reader = checksJsonReader(study_path)
            if checks_json_reader.json_file_exists():
                checks_json_reader.read()
                self.pairs_.extend(checks_json_reader.get_checks())
                self.test_ids_.extend(checks_json_reader.get_ids())

    def pairs(self):
        return self.pairs_

    def testIds(self):
        return self.test_ids_


"""
# ================
# For validation
# ================
ROOT_FOLDER = Path('../../resources/study-collection-sample').resolve()

study_paths = [ROOT_FOLDER / "short-tests" / "001 One node - passive"]
json_collector = jsonCollector(study_paths)
json_collector.collect()

print("pairs :", json_collector.pairs())
print("testIds :", json_collector.testIds())
"""