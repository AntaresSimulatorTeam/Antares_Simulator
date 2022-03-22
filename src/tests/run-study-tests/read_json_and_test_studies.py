from fixtures import *
from parse_studies.look_for_studies import look_for_studies
from parse_studies.json_collector import jsonCollector
from check_on_results.create_checks import create_checks


ROOT_FOLDER = Path('../resources/study-collection-sample').resolve()

study_paths = look_for_studies(ROOT_FOLDER)
json_collector = jsonCollector(study_paths)
json_collector.collect()

@pytest.mark.parametrize('study_path,test_check_data', json_collector.pairs(), ids=json_collector.testIds())
def test_on_all_studies(study_path, test_check_data, check_runner):
    list_of_checks = create_checks(study_path, test_check_data, simulation=check_runner.get_simulation())

    checks = check_list()
    for c in list_of_checks:
        checks.add(check = c)
    check_runner.run(checks)



