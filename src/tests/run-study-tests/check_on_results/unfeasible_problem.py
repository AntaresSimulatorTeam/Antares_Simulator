
from check_on_results.check_general import check_interface, check_list
from utils.find_output import find_output_folder
from utils.assertions import raise_assertion

def find_logs_file(path_to_output_folder):
    found_files = []
    for path in path_to_output_folder.rglob('simulation.log'):
        found_files.append(path)
    if len(found_files) == 0:
        raise_assertion("Found no log file for simulation")
    elif len(found_files) > 1:
        raise_assertion("Found too many log files for simulation")
    return found_files[0]


class weeks_in_year:
    def __init__(self, year, weeks = []):
        self.year = year
        self.weeks = weeks

class check_logs_content(check_interface):
    def __init__(self, study_path, log_level="warns", faulty_weeks=[]):
        check_interface.__init__(self, study_path)
        self.log_level = log_level
        self.expected_faulty_weeks = faulty_weeks

        if log_level not in ["warns", "fatal"]:
            raise_assertion("Unknown log level : %s" % log_level)

    def need_output_results(self):
        return False

    def run(self):
        path_to_output_folder = find_output_folder(self.study_path)
        path_to_logs = find_logs_file(path_to_output_folder)


class behavior_flag:
    def __init__(self, raw_behavior):
        self.raw_behavior = raw_behavior
        self.split_behavior = None
        self.error_or_warning_converter = {'warning' : 'warns', 'error' : 'fatal'}

    def check_conformity(self):
        self.split_behavior = self.raw_behavior.split('-')
        if len(self.split_behavior) != 2:
            raise_assertion("behavior incorrect : %s" % self.raw_behavior)
        if self.split_behavior[0] not in ['error', 'warning']:
            raise_assertion("behavior incorrect : %s" % self.raw_behavior)
        if self.split_behavior[1] not in ['dry', 'verbose']:
            raise_assertion("behavior incorrect : %s" % self.raw_behavior)

    def contains_verbose(self):
        if self.split_behavior[1] == 'verbose':
            return True
        return False


class unfeasible_problem(check_interface):
    def __init__(self, study_path, behavior="error-verbose", checks_on_weeks = []):
        check_interface.__init__(self, study_path)
        self.behavior = behavior_flag(behavior)
        self.behavior.check_conformity()
        self.error_or_warning_keyword = self.behavior.error_or_warning_keyword()

        self.checks_on_weeks = checks_on_weeks

        self.checks = check_list()
        self.fill_check_list()

    def need_output_results(self):
        return self.checks.output_results_needed()

    def run(self):
        # To be done
        print("running check : %s" % self.__class__.__name__)
        self.checks.run()

    def fill_check_list(self):
        self.checks.add(check = check_logs_content(self.study_path, self.error_or_warning_keyword, self.checks_on_weeks))
        # To be finished
        # self.checks.add(self.)
