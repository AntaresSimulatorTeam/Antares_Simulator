import pytest

from actions_on_study.results_remover import results_remover
from check_on_results.check_general import check_list

class check_handler:
    def __init__(self, results_remover):
        self.results_remover = results_remover
        self.checks = check_list()

    def run(self, simulation, checks):
        self.checks = checks
        simulation.run()
        checks.run()

    def teardown(self):
        study_modifiers = self.checks.study_modifiers()
        for modifier in study_modifiers:
            modifier.back_to_initial_state()

        self.results_remover.run()

# ================
# Fixtures
# ================
@pytest.fixture()
def study_path(request):
    return request.param

@pytest.fixture
def resultsRemover(study_path):
    return results_remover(study_path)

@pytest.fixture
def check_runner(resultsRemover):
    my_check_handler = check_handler(resultsRemover)
    try:
        yield my_check_handler
    finally:
        my_check_handler.teardown()
