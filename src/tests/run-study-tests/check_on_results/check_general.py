import sys
import abc


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