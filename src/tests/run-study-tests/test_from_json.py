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
    name: Path
    duration: float 
    memory: int
class BadBenchmarkFile(Exception):
    pass
class CustomBenchmark:
    def __init__(self, out_file: Path, data: CustomBenchmarkData) -> None:
        self.out_file = out_file
        self.data = data
        self.name = Path(self.data.name).name
    
    def dump_json(self):
        lines = []
        if (Path(self.out_file).exists()):
            with open(self.out_file, 'r') as in_file:
                for line in in_file: 
                    if not line.isspace(): 
                        lines.append(line) 
            ## checks
            assert(lines[0][0]=='[')
            assert(lines[-1][-1]==']')
            lines[-1]=lines[-1][:-1]
        else:
            lines.append("[\n")
                 
    
        lines.append(self.duration())
        lines.append(",\n")
        lines.append(self.memory())
        lines.append(",\n")

        lines.append("]")
        with open(self.out_file, "w") as output:
            output.writelines(lines)
            
             
    def duration(self):
        return str(json.dumps( {
                "name" : self.name+"_duration",
                "value": self.data.duration,
                "unit": "s"
            }))
        
             
    def memory(self):
        return str(json.dumps({
                "name" : self.name+"_memory",
                "value": self.data.memory,
                "unit": "mb"
                }))

def my_test(study_path, test_check_data, check_runner):
    checks = create_checks(study_path, test_check_data, simulation=check_runner.get_simulation())
    check_runner.run(checks)

@pytest.mark.json
@pytest.mark.parametrize('study_path, test_check_data', json_collector.pairs(), ids=json_collector.testIds())
def test(study_path, test_check_data, check_runner, do_benchmark, custom_benchmark_json):
    if do_benchmark:
        # benchmark(my_test, study_path, test_check_data, check_runner)
        my_test(study_path, test_check_data, check_runner)
    
        CustomBenchmark(custom_benchmark_json, CustomBenchmarkData(study_path, random.uniform(0, 100), random.randint(0,10))).dump_json()
    else:
        my_test(study_path, test_check_data, check_runner)
