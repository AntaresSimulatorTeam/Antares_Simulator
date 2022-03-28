from check_on_results.check_general import check_interface
from utils.assertions import raise_assertion

class simulation_success(check_interface):
    def __init__(self, study_path, simulation, success_expected = True):
        super().__init__(study_path)

        self.simulation = simulation
        self.simulation.no_exception_raised_at_failure()

        self.success_expected = success_expected

    def run(self):
        super().print_check_name()
        success = self.simulation.success()
        if success and not self.success_expected:
            raise_assertion('Simulation succeeded. Failure was expected')
        if not success and self.success_expected:
            raise_assertion('Simulation failed. Success was expected')

    def check_name(self):
        return "simulation success"
