import sys
import abc


# Base class for any check class
class check_interface(metaclass=abc.ABCMeta):
    def __init__(self, study_path):
        self.study_path = study_path
        self.study_modifiers_ = []

    @abc.abstractmethod
    def run(self):
        pass

    @abc.abstractmethod
    def check_name(self):
        pass

    def print_check_name(child_run):
        def wrapper(self):
            print("Running check : %s" % self.check_name())
            child_run(self)
        return wrapper

    def study_modifiers(self):
        return self.study_modifiers_


class check_list:
    def __init__(self):
        self.check_objects = []

    def add(self, check, system = 'any_system'):
        if system not in [sys.platform, 'any_system']:
            return
        self.check_objects.append(check)

    def study_modifiers(self):
        modifiers = []
        for check in self.check_objects:
            modifiers.extend(check.study_modifiers())
        return modifiers

    def run(self):
        for check in self.check_objects:
            check.run()