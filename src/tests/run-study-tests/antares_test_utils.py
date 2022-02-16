import sys
import abc


#   ==========
#   All checks
#   ==========
# Abstract base class
class check_interface(metaclass=abc.ABCMeta):
    def __init__(self, study_path):
        self.study_path = study_path

    @abc.abstractmethod
    def need_output_results(self):
        pass

    @abc.abstractmethod
    def run(self):
        pass

class output_compare(check_interface):
    def need_output_results(self):
        return True

    def run(self):
        # To be done
        pass

class integrity_compare(check_interface):
    def need_output_results(self):
        return False

    def run(self):
        # To be done
        pass

class reservoir_levels(check_interface):
    def need_output_results(self):
        return False

    def run(self):
        # To be done
        pass

class unfeasible_problem(check_interface):
    def need_output_results(self):
        return False

    def run(self):
        # To be done
        pass

#   ==============
#   Checks factory
#   ==============
class check_factory:
    def __init__(self):
        self.study_path = None
        print('check_factory constructor')

    def set_study_path(self, study_path):
        self.study_path = study_path

    def print_study_path(self):
        print('\nstudy path : %s' % self.study_path)

    def get(self, check_type):
        print('get with : %s' % check_type)
        if check_type == 'output_compare':
            return output_compare(self.study_path)
        elif check_type == 'integrity_compare':
            return integrity_compare(self.study_path)
        elif check_type == 'reservoir_levels':
            return reservoir_levels(self.study_path)
        elif check_type == 'unfeasible_problem':
            return unfeasible_problem(self.study_path)
        else:
            return None


#   =================
#   Running the study
#   =================
# Abstract class
class study_run_interface(metaclass=abc.ABCMeta):
    def __init__(self, study_path, solver_path, use_ortools, ortools_solver):
        self.study_path = study_path
        self.solver_path = solver_path
        self.use_ortools = use_ortools
        self.ortools_solver = ortools_solver

    @abc.abstractmethod
    def run(self):
        pass

class study_run(study_run_interface):
    def __init__(self, study_path, solver_path, use_ortools, ortools_solver):
        study_run_interface.__init__(study_path, solver_path, use_ortools, ortools_solver)

    def run(self):
        study_run_interface.run()


class study_run_print_outputs(study_run_interface):
    def __init__(self, study_path, solver_path, use_ortools, ortools_solver):
        study_run_interface.__init__(study_path, solver_path, use_ortools, ortools_solver)

    def run(self):
        # Enabling outputs here
        study_run_interface.run()
        # Disabling outputs here

#   Execution factory
#   -----------------
class study_execution_factory:
    def __init__(self, solver_path, use_ortools, ortools_solver):
        self.study_path = None
        self.solver_path = solver_path
        self.use_ortools = use_ortools
        self.ortools_solver = ortools_solver

    def set_study_path(self, study_path):
        self.study_path = study_path

    def get(checks):
        if checks.output_results_needed():
            return



#   ==========
#   Check list
#   ==========
class check_list:
    def __init__(self):
        self.check_objects = []

    def add(self, check, system = 'any_system'):
        if system not in [sys.platform, 'any_system']:
            return
        self.check_objects.append(check)


    def output_results_needed(self):
        for check in self.check_objects:
            if check.need_output_results():
                return True
        return False

    def run(self):
        for check in self.check_objects:
            check.run()

