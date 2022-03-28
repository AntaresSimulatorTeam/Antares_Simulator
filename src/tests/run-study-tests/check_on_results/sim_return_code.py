from check_on_results.check_general import check_interface
from utils.assertions import check

class simulation_success(check_interface):
    def __init__(self, study_path, simulation, success_expected = True):
        super().__init__(study_path)

        self.simulation = simulation
        self.simulation.no_exception_raised_on_failure()

        self.success_expected = success_expected

    def run(self):
        super().print_check_name()
        success = self.simulation.success()
        check(not self.success_expected or success, 'Simulation failed, but success was expected.')
        check(self.success_expected or not success, 'Simulation succeeded, but failure was expected.')

    def check_name(self):
        return "simulation success"
