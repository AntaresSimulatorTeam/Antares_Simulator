from fixtures import *
from parse_studies.look_for_studies import look_for_studies
from parse_studies.json_collector import jsonCollector
from check_on_results.create_checks import create_checks



ROOT_FOLDER = Path('../resources/batches').resolve()


# Search for studies inside directory ROOT_FOLDER and collects all their paths.
# Each study is supposed to hold a check-config.json file, containing
# all data to make particular checks on the associated study.
study_paths = look_for_studies(ROOT_FOLDER)

# Collects json files in each study and retrieves the checks data from each of them.
json_collector = jsonCollector(study_paths)
json_collector.collect()


    
class TestFromJson:
    
    @pytest.fixture(autouse=True)    
    def setup_method_fixture(self, do_benchmark, custom_benchmark_json):
        self.do_benchmark = False
        self.list_of_benchmark = []
        self.custom_benchmark_json = custom_benchmark_json
        self.do_benchmark = do_benchmark
        if (self.do_benchmark):
            with open(self.custom_benchmark_json, "w") as out:
                    out.write("[\n")

    def teardown_method(self):
        if self.do_benchmark:
            with open(self.custom_benchmark_json, "+a") as out:
                out.write("\n]")

    @pytest.mark.json
    @pytest.mark.parametrize('study_path, test_check_data', json_collector.pairs(), ids=json_collector.testIds())
    def test(self, study_path, test_check_data, check_runner):
        checks = create_checks(study_path, test_check_data, simulation=check_runner.get_simulation())
        check_runner.run(checks)


