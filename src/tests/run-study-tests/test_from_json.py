from dataclasses import dataclass
from fixtures import *
from parse_studies.look_for_studies import look_for_studies
from parse_studies.json_collector import jsonCollector
from check_on_results.create_checks import create_checks
import json

#tmp
import random


ROOT_FOLDER = Path('../resources/batches').resolve()


# Search for studies inside directory ROOT_FOLDER and collects all their paths.
# Each study is supposed to hold a check-config.json file, containing
# all data to make particular checks on the associated study.
study_paths = look_for_studies(ROOT_FOLDER)

# Collects json files in each study and retrieves the checks data from each of them.
json_collector = jsonCollector(study_paths)
json_collector.collect()

@dataclass
class CustomBenchmarkData:
    name: str
    duration: float 
    memory: int

class CustomBenchmark:
    def __init__(self, json_file: Path, data: CustomBenchmarkData) -> None:
        self.json_file = json_file
        self.data = data
        self.name = self.data.name

    def dump_json(self):
         to_file = []
        
         to_file.append(self.duration())
         to_file.append(self.memory())
         with open(self.json_file) as output:
            json.dump(to_file, output, indent=4)
             
    def duration(self):
        return {
                "name" : self.name,
                "value": self.data.duration,
                "unit": "s"
                }
             
    def memory(self):
        return {
                "name" : self.name,
                "value": self.data.memory,
                "unit": "mb"
                }

def my_test(study_path, test_check_data, check_runner):
    checks = create_checks(study_path, test_check_data, simulation=check_runner.get_simulation())
    check_runner.run(checks)

@pytest.mark.json
@pytest.mark.parametrize('study_path, test_check_data', json_collector.pairs(), ids=json_collector.testIds())
def test(study_path, test_check_data, check_runner, do_benchmark, benchmark_json):
    if do_benchmark:
        # benchmark(my_test, study_path, test_check_data, check_runner)
        my_test(study_path, test_check_data, check_runner)
    
        CustomBenchmark(benchmark_json, CustomBenchmarkData(study_path, random.float(0, 100), random.randint(0,10)))
    else:
        my_test(study_path, test_check_data, check_runner)
