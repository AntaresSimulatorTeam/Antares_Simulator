from check_on_results.check_general import check_interface
from utils.assertions import raise_assertion

class simulation_success(check_interface):
    def __init__(self, study_path, simulation, success_expected = True):
        check_interface.__init__(self, study_path)

        self.simulation = simulation
        self.simulation.no_exception_raised_at_failure()

        self.success_expected = success_expected

    def run(self):
        print("running check : %s" % self.__class__.__name__)
        success = self.simulation.success()
        if success and not self.success_expected:
            raise_assertion('Simulation succeeded. Failure was expected')
        if not success and self.success_expected:
            raise_assertion('Simulation failed. Success was expected')
