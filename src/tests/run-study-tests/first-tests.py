import pytest
from pathlib import Path

from check_on_results.check_general import check_list
from check_on_results.tolerances import get_tolerances
from check_on_results.output_compare import output_compare
from check_on_results.integrity_compare import integrity_compare
from check_on_results.check_hydro_level import check_hydro_level
from check_on_results.unfeasible_problem import unfeasible_problem

from fixtures import *

ALL_STUDIES_PATH = Path('../resources/Antares_Simulator_Tests').resolve()

# --------------------------------------------------------------
# Example of a test with output AND check integrity comparisons
# --------------------------------------------------------------

@pytest.mark.parametrize('study_path', [ALL_STUDIES_PATH / "short-tests" / "001 One node - passive"], indirect=True)
def test_1(study_path, check_runner):
    checks = check_list()
    checks.add(check = output_compare(study_path), system = 'win32')
    checks.add(check = integrity_compare(study_path))
    check_runner.run(checks)



@pytest.mark.short
@pytest.mark.parametrize('study_path', [ALL_STUDIES_PATH / "short-tests" / "002 Thermal fleet - Base"], indirect=True)
def test_2(study_path, check_runner):
    checks = check_list()
    checks.add(check = integrity_compare(study_path), system = 'win32')
    checks.add(check = unfeasible_problem(study_path))
    check_runner.run(checks)


# --------------------------------------------------------------
# Example of a test with output comparison implying tolerances
# --------------------------------------------------------------
@pytest.mark.short
@pytest.mark.parametrize('study_path', [ALL_STUDIES_PATH / "short-tests" / "playlist-psp-misc-ndg"], indirect=True)
def test_3(study_path, check_runner):
    # Build tolerances for an "output comparison"
    var_collection = ['CCGT_new', 'CCGT-old-2', 'CONG. FEE (ABS.)', 'CONG. FEE (ALG.)', 'gas_ccgt new', 'gas_ccgt old 1']
    var_collection.extend(['gas_ccgt old 2', 'gas_conventional old 1', 'gas_ocgt new', 'gas_ocgt old', 'hard coal_new'])
    var_collection.extend(['hard coal_old 1', 'HURDLE COST', 'LIGNITE', 'lignite_new', 'Lignite-new', 'lignite_old 1'])
    var_collection.extend(['lignite_old 2', 'Lignite-old-1', 'Lignite-old-2', 'NP COST', 'nuclear_nuclear', 'OCGT_new'])
    var_collection.extend(['OCGT-old', 'SOLAR'])

    tolerances = get_tolerances()
    tolerances.set_absolute(var_collection, 1)

    # Add an "output comparison" to check list
    checks = check_list()
    checks.add(check=output_compare(study_path, tolerances), system='win32')

    check_runner.run(checks)

@pytest.mark.short
@pytest.mark.parametrize('study_path', [ALL_STUDIES_PATH / "short-tests" / "hydro initialization 1"], indirect=True)
def test_check_initial_hydro_level(study_path, check_runner):
    first_hour_of_january = 0
    checks = check_list()
    checks.add(check=check_hydro_level(study_path, date_in_hours=first_hour_of_january, level=30, tolerance=.05))
    check_runner.run(checks)


@pytest.mark.short
@pytest.mark.parametrize('study_path', [ALL_STUDIES_PATH / "short-tests" / "hydro initialization 2"], indirect=True)
def test_check_hydro_level_at_1st_hour_of_march(study_path, check_runner):
    first_hour_of_march = 1417
    checks = check_list()
    checks.add(check=check_hydro_level(study_path, date_in_hours=first_hour_of_march, level=30, tolerance=.05))
    check_runner.run(checks)


@pytest.mark.short
@pytest.mark.parametrize('study_path', [ALL_STUDIES_PATH / "short-tests" / "hydro preference 1"], indirect=True)
def test_check_hydro_level_at_last_hour_of_simulation(study_path, check_runner):
    last_hour_of_simulation = -1
    checks = check_list()
    checks.add(check=check_hydro_level(study_path, date_in_hours=last_hour_of_simulation, level=30.46, tolerance=.05))
    check_runner.run(checks)
