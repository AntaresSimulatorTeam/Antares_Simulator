from check_on_results.check_general import check_interface
from check_decorators.print_name import printNameDecorator
from utils.assertions import check

@printNameDecorator
class simulation_success(check_interface):
    def __init__(self, study_path, simulation, success_expected = True):
        super().__init__(study_path)

        self.simulation = simulation
        self.simulation.no_exception_raised_on_failure()

        self.success_expected = success_expected

    def run(self):
        success = self.simulation.success()
        check(not self.success_expected or success, 'Simulation failed, but success was expected.')
        check(self.success_expected or not success, 'Simulation succeeded, but failure was expected.')

    def name(self):
        return "simulation success"
