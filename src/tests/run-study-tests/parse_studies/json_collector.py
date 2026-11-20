from parse_studies.configuration import load_check_config


def contains_json_config(study_path):
    return (study_path / "check-config.json").exists()


class jsonCollector:
    def __init__(self, study_paths):
        self.study_paths_ = study_paths
        self.pairs_ = []
        self.test_ids_ = []

    def collect(self):
        for study_path in self.study_paths_:
            if not contains_json_config(study_path):
                print(f"Skipping checks on '{study_path}': check-config.json does not exist")
                continue
            for test in load_check_config(study_path):
                study_path_id = f"{study_path.parent.name} / {study_path.name}"
                self.test_ids_.append(f"{test['name']}  ({study_path_id})")
                self.pairs_.append((study_path, test))

    def pairs(self):
        return self.pairs_

    def testIds(self):
        return self.test_ids_
