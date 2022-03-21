from check_on_results.check_general import check_list

from check_on_results.integrity_compare import integrity_compare

from fixtures import *
from parse_studies.look_for_studies import look_for_studies


ROOT_FOLDER = Path('d:/') / 'Users' / 'pierregui' / "Desktop" / "python-test" / "valid-defaillance"
list_of_studies = look_for_studies(ROOT_FOLDER)


@pytest.mark.parametrize('study_path', list_of_studies, indirect=True)
def test_on_all_studies(study_path, check_runner):
    checks = check_list()
    checks.add(check = integrity_compare(study_path))
    check_runner.run(checks)


