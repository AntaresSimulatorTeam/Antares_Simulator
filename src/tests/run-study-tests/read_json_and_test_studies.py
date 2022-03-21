from fixtures import *
from parse_studies.look_for_studies import look_for_studies
from parse_studies.get_check_info import get_studies_check_info
from check_on_results.create_checks import create_checks


ROOT_FOLDER = Path('../resources/study-collection-sample').resolve()

list_of_studies = look_for_studies(ROOT_FOLDER)
studies_check_info = get_studies_check_info(list_of_studies)

@pytest.mark.parametrize('study_path,study_check_data', studies_check_info)
def test_on_all_studies(study_path, study_check_data, check_runner):
    # Create a check list from checks data
    # print("study_path :", study_path)
    # print("study_check_data :", study_check_data)
    list_of_checks = create_checks(study_path, study_check_data, simulation=check_runner.get_simulation())

    checks = check_list()
    for c in list_of_checks:
        checks.add(check = c)
    check_runner.run(checks)


