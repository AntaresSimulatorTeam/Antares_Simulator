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
        print("running check : %s" % self.__class__.__name__)

class integrity_compare(check_interface):
    def need_output_results(self):
        return False

    def run(self):
        # To be done
        print("running check : %s" % self.__class__.__name__)

class reservoir_levels(check_interface):
    def need_output_results(self):
        return False

    def run(self):
        # To be done
        print("running check : %s" % self.__class__.__name__)

class unfeasible_problem(check_interface):
    def need_output_results(self):
        return False

    def run(self):
        # To be done
        print("running check : %s" % self.__class__.__name__)

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
        # print("output_results_needed :", self.check_objects)
        for check in self.check_objects:
            if check.need_output_results():
                return True
        return False

    def run(self):
        for check in self.check_objects:
            check.run()



