import json
from jsonschema import validate, Draft7Validator
from pathlib import Path
import os.path

class error:
    def __init__(self, type, items = []):
        self.type = type
        self.items = items

    def print(self):
        print("  Error type : %s" % self.type)
        for item in self.items:
            print("    ==> %s" % item)



class checksRetriever:
    def __init__(self, study_path):
        self.study_path = study_path

        self.json_file = study_path / "check-config.json"
        self.json_file_content = None

        self.json_schema_file = "./parse_studies/json_schema.json"
        self.json_schema_content = None

        self.test_pairs = []
        self.test_ids = []
        self.errors = []

    def run(self):
        if not self.json_file_exists():
            return

        if not self.read_json_file():
            return

        if not self.read_json_schema():
            return

        if not self.validate_json():
            return

        for test in self.json_file_content:
            study_path_id = self.study_path.parts[-2] + " / " + self.study_path.parts[-1]
            self.test_ids.append(test["name"] + "  (%s)" % study_path_id)
            self.test_pairs.append((self.study_path, test["checks"]))

    def json_file_exists(self) -> bool:
        if not os.path.isfile(self.json_file):
            self.errors.append(error("json file does not exist"))
            return False
        return True

    def read_json_file(self) -> bool:
        try:
            with open(self.json_file, "r") as json_file:
                self.json_file_content = json.load(json_file)
        except json.decoder.JSONDecodeError as err:
            self.errors.append(error("json file ill formed", [err]))
            return False
        return True

    def read_json_schema(self) -> bool:
        try:
            with open(self.json_schema_file, "r") as file:
                self.json_schema_content = json.load(file)
        except json.decoder.JSONDecodeError as err:
            self.errors.append(error("json schema file ill formed", [err]))
            return False
        return True

    def validate_json(self) -> bool:
        v = Draft7Validator(self.json_schema_content)
        errors = [err.message for err in sorted(v.iter_errors(self.json_file_content), key=str)]
        if len(errors) > 0:
            self.errors.append(error("json file not valid under schema", errors))
            return False
        return True

    def has_errors(self) -> bool:
        if len(self.errors) > 0:
            return True
        return False

    def print_errors(self):
        print("\n---")
        print("Skipping checks on : '%s', because :" % os.path.basename(self.study_path))
        for err in self.errors:
            err.print()

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
            checks_retriever = checksRetriever(study_path)
            checks_retriever.run()
            if checks_retriever.has_errors():
                checks_retriever.print_errors()
            else:
                self.pairs_.extend(checks_retriever.get_checks())
                self.test_ids_.extend(checks_retriever.get_ids())

    def pairs(self):
        return self.pairs_

    def testIds(self):
        return self.test_ids_



# ================
# For validation
# ================
ROOT_FOLDER = Path('../../resources/study-collection-sample').resolve()
# print("ROOT_FOLDER : %s" % ROOT_FOLDER)

if __name__ == "__main__":
    study_paths = [ROOT_FOLDER / "short-tests" / "hydro initialization 1"]
    json_collector = jsonCollector(study_paths)
    json_collector.collect()
