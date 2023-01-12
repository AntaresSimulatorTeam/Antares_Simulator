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

@pytest.mark.json
@pytest.mark.parametrize('study_path, test_check_data', json_collector.pairs(), ids=json_collector.testIds())
def test(study_path, test_check_data, check_runner):
    checks = create_checks(study_path, test_check_data, simulation=check_runner.get_simulation())
    check_runner.run(checks)



