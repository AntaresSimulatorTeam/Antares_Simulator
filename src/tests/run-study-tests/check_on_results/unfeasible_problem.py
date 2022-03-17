
from check_on_results.check_general import check_interface, check_list
from utils.find_output import find_output_folder
from actions_on_study.study_modifier import study_modifier
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

    def run(self):
        path_to_output_folder = find_output_folder(self.study_path)
        path_to_log_file = find_logs_file(path_to_output_folder)

        # Build lines to be found in logs file
        lines_to_find = []
        match_lines = []
        for weeks_of_year in self.expected_faulty_weeks:
            year = weeks_of_year.year
            week_label = "week" if len(weeks_of_year.weeks) == 1 else "weeks"
            concatenated_weeks = ""
            for week_number in weeks_of_year.weeks:
                concatenated_weeks += " " + str(week_number)
            lines_to_find.append("[solver][" + self.log_level + "] Year " + str(year) + " failed at "
                                 + week_label + concatenated_weeks + ".")

            match_lines.append(False)

        # Search expected lines in log file content and store matches
        log_file = open(str(path_to_log_file), 'r')
        for line in log_file:
            for i in range(len(lines_to_find)):
                if lines_to_find[i] in line:
                    match_lines[i] = True

        # Report of lines not found
        for i in range(len(lines_to_find)):
            assert match_lines[i], "'" + lines_to_find[i] + "'" + " log not found in simulation.log"



class behavior_flag:
    error_or_warning_converter = {'warning': 'warns', 'error': 'fatal'}

    def __init__(self, raw_behavior):
        self.raw_behavior = raw_behavior
        self.split_behavior = None

    def check_conformity(self):
        self.split_behavior = self.raw_behavior.split('-')
        if len(self.split_behavior) != 2:
            raise_assertion("behavior incorrect : %s" % self.raw_behavior)
        if self.split_behavior[0] not in ['error', 'warning']:
            raise_assertion("behavior incorrect : %s" % self.raw_behavior)
        if self.split_behavior[1] not in ['dry', 'verbose']:
            raise_assertion("behavior incorrect : %s" % self.raw_behavior)

    def error_or_warning_keyword(self):
        return self.error_or_warning_converter[self.split_behavior[0]]

    def contains_verbose(self):
        if self.split_behavior[1] == 'verbose':
            return True
        return False


class unfeasible_problem(check_interface):
    def __init__(self, study_path, new_behavior="error-verbose", checks_on_weeks = []):
        check_interface.__init__(self, study_path)

        # Unfeasible problem behavior flag
        self.new_behavior_flag = behavior_flag(new_behavior)
        self.new_behavior_flag.check_conformity()
        self.error_or_warning_keyword = self.new_behavior_flag.error_or_warning_keyword()
        behavior_flag_modifier = study_modifier(self.study_path, study_parameter="include-unfeasible-problem-behavior",
                                                new_value=new_behavior, study_file_id="general")
        behavior_flag_modifier.activate()
        self.study_modifiers_.append(behavior_flag_modifier)

        # List of weeks on which we make checks
        self.checks_on_weeks = checks_on_weeks

        # We have a list of checks to run here
        self.checks = check_list()
        self.fill_check_list()

    def need_output_results(self):
        return self.checks.output_results_needed()

    def run(self):
        # To be done
        print("running check : %s" % self.__class__.__name__)
        self.checks.run()

    def fill_check_list(self):
        self.checks.add(check = check_logs_content(self.study_path, self.error_or_warning_keyword,
                                                   self.checks_on_weeks))
        # To be finished
        # self.checks.add(self.)
