import json
from jsonschema import validate
import os.path

class checksJsonReader:
    def __init__(self, study_path):
        self.study_path = study_path
        self.json_file = study_path / "check-config.json"

        self.test_pairs = []
        self.test_ids = []

    def json_file_exists(self):
        return os.path.isfile(self.json_file)

    def read(self):
        with open(self.json_file, "r") as json_file:
            test_collection = json.load(json_file)

        with open("./parse_studies/json_schema.json", "r") as file:
            valid_dictionary = json.load(file)

        validate_json(test_collection, valid_dictionary)

        for test in test_collection:
            self.test_ids.append(test["name"])
            self.test_pairs.append((self.study_path, test["checks"]))

    def get_ids(self):
        return self.test_ids
    def get_checks(self):
        return self.test_pairs


def validate_json(data_to_validate, valid_dictionary):
    try:
        validate(data_to_validate, valid_dictionary)
    except Exception as valid_err:
        print("Validation KO: {}".format(valid_err))
        raise valid_err


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