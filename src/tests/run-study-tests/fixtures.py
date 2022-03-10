import pytest

from actions_on_study.study_run import *
from actions_on_study.results_remover import *
from actions_on_study.print_results_handler import *

class check_handler:
    def __init__(self, simulation, print_results_handler, results_remover):
        self.simulation = simulation
        self.print_results_handler = print_results_handler
        self.results_remover = results_remover

    def run(self, checks):
        self.print_results_handler.enable_if_needed(checks)
        self.simulation.run()
        checks.run()

    def clean(self):
        self.print_results_handler.back_to_previous_state()
        self.results_remover.run()

# ================
# Fixtures
# ================
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
def printResults(study_path):
    return print_results_handler(study_path)


@pytest.fixture(autouse=True)
def check_runner(simulation, printResults, resutsRemover):
    # Actions done before the current test
    # print("\nTest begins")
    my_check_handler = check_handler(simulation, printResults, resutsRemover)

    # Running the current test here
    yield my_check_handler

    # Teardown : actions done after the current test
    my_check_handler.clean()
    # print('End of test')