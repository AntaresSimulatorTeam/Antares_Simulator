import pytest

from check_on_results.compare_mps_files import compare_mps_files
from fixtures import *

ROOT_FOLDER = Path('../resources/batches').resolve()
list_studies = [ROOT_FOLDER / "025_mps-week", ROOT_FOLDER / "025_mps-day"]

# --------------------------------------------------------------
# Test content of mps files when generated
# --------------------------------------------------------------
@pytest.mark.mps
@pytest.mark.parametrize('study_path', list_studies, indirect=True)
def test_mps_files(study_path, check_runner):
    checks = check_list()
    checks.add(check = compare_mps_files(study_path))
    check_runner.run(checks)