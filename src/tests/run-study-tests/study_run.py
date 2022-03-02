import abc

# Abstract base class
class study_run_interface(metaclass=abc.ABCMeta):
    def __init__(self, study_path, solver_path, use_ortools, ortools_solver):
        self.study_path = study_path
        self.solver_path = solver_path
        self.use_ortools = use_ortools
        self.ortools_solver = ortools_solver

    @abc.abstractmethod
    def run(self):
        pass

    def base_run(self):
        print("   Base run")

class study_run(study_run_interface):
    def run(self):
        self.base_run()


class study_run_print_outputs(study_run_interface):
    def run(self):
        print("   Enabling the output")
        self.base_run()
        print("   Disabling the output")

#   -----------------
#   Execution factory
#   -----------------
class study_execution_factory:
    def __init__(self, study_path, solver_path, use_ortools, ortools_solver):
        print("\nstudy path : %s" % study_path)
        self.study_path = study_path
        self.solver_path = solver_path
        self.use_ortools = use_ortools
        self.ortools_solver = ortools_solver

    def get(self, checks):
        if checks.output_results_needed():
            return study_run_print_outputs(self.study_path, self.solver_path, self.use_ortools, self.ortools_solver)
        else:
            return study_run(self.study_path, self.solver_path, self.use_ortools, self.ortools_solver)