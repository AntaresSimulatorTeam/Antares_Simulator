from pathlib import Path

# Collects studies that are run in cucumber feature files
class cucumberCollector:
    def __init__(self, cucumber_path : Path):
        self.cucumber_path = cucumber_path

    def collect(self):
        feature_files = []
        for path in self.cucumber_path.rglob("*"):
            if not path.is_dir() and path.name.endswith(".feature"):
                feature_files.append(path)
        studies = []
        for feature in feature_files:
            with open(feature, "r") as f:
                for line in f.readlines():
                    if "Given the solver study path is \"Antares_Simulator_Tests_NR/" in line:
                        study = line.strip()
                        study = study.replace("Given the solver study path is \"Antares_Simulator_Tests_NR/", "")
                        study = study.replace("\"", "")
                        studies.append(study)
        return studies