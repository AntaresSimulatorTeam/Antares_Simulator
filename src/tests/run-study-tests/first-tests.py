import pytest
from check_on_results.check_general import check_list

from check_on_results.tolerances import get_tolerances
from check_on_results.output_compare import output_compare

from check_on_results.integrity_compare import integrity_compare
from check_on_results.reservoir_levels import reservoir_levels
from check_on_results.unfeasible_problem import unfeasible_problem

from study_run import *
from results_remover import *
from print_results_handler import *

ALL_STUDIES_PATH = Path('../resources/Antares_Simulator_Tests').resolve()

@pytest.fixture()
def study_path(request):
    return request.param

@pytest.fixture
def resutsRemover(study_path):
    return resuts_remover(study_path)

@pytest.fixture
def simulation(study_path, solver_path, use_ortools, ortools_solver):
    return study_run(study_path, solver_path, use_ortools, ortools_solver)

@pytest.fixture
def checks():
    return check_list()

@pytest.fixture
def printResults(study_path):
    return print_results_handler(study_path)

@pytest.fixture(autouse=True)
def setup(simulation, printResults, checks, resutsRemover):
    # Actions done before the current test
    # ==> nothing to run here
    print("\nTest begins")

    # Running the current test here
    yield

    print() # To next line after current test status (PASSED or ERROR) is printed

    # Teardown : actions done after the current test
    printResults.enable_if_needed(checks)
    simulation.run()
    printResults.back_to_previous_state()

    # ... Make all checks of the current test
    checks.run()

    # ... Remove results on disk
    resutsRemover.run()

    print('End of test')


# --------------------------------------------------------------
# Example of a test with output AND check integrity comparisons
# --------------------------------------------------------------
@pytest.mark.short
@pytest.mark.parametrize('study_path', [ALL_STUDIES_PATH / "short-tests" / "001 One node - passive"], indirect=True)
def test_1(checks, study_path):
    checks.add(check = output_compare(study_path), system = 'win32')
    checks.add(check = integrity_compare(study_path))
    checks.add(check = reservoir_levels(study_path))


@pytest.mark.short
@pytest.mark.parametrize('study_path', [ALL_STUDIES_PATH / "short-tests" / "002 Thermal fleet - Base"], indirect=True)
def test_2(checks, study_path):
    checks.add(check = integrity_compare(study_path), system = 'win32')
    checks.add(check = unfeasible_problem(study_path))

# --------------------------------------------------------------
# Example of a test with output comparison implying tolerances
# --------------------------------------------------------------
@pytest.mark.short
@pytest.mark.parametrize('study_path', [ALL_STUDIES_PATH / "short-tests" / "playlist-psp-misc-ndg"], indirect=True)
def test_3(checks, study_path):
    # Build tolerances for an "output comparison"
    var_collection = ['CCGT_new', 'CCGT-old-2', 'CONG. FEE (ABS.)', 'CONG. FEE (ALG.)', 'gas_ccgt new', 'gas_ccgt old 1']
    var_collection.extend(['gas_ccgt old 2', 'gas_conventional old 1', 'gas_ocgt new', 'gas_ocgt old', 'hard coal_new'])
    var_collection.extend(['hard coal_old 1', 'HURDLE COST', 'LIGNITE', 'lignite_new', 'Lignite-new', 'lignite_old 1'])
    var_collection.extend(['lignite_old 2', 'Lignite-old-1', 'Lignite-old-2', 'NP COST', 'nuclear_nuclear', 'OCGT_new'])
    var_collection.extend(['OCGT-old', 'SOLAR'])

    tolerances = get_tolerances()
    tolerances.set_absolute(var_collection, 1)

    # var_collection = ['gas_ccgt old 2']
    # tolerances.set_relative(var_collection, 30.)

    # Add an "output comparison" to check list
    checks.add(check=output_compare(study_path, tolerances), system='win32')
