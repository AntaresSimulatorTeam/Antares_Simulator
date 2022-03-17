
from check_on_results.check_general import check_interface, check_list
from check_on_results.check_logs_content import check_logs_content
from actions_on_study.study_modifier import study_modifier
from utils.assertions import raise_assertion


class weeks_in_year:
    def __init__(self, year, weeks = []):
        self.year = year
        self.weeks = weeks


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
        behavior_flag_modifier = study_modifier(self.study_path, study_parameter="include-unfeasible-problem-behavior",
                                                new_value=new_behavior, study_file_id="general")
        behavior_flag_modifier.activate()
        self.study_modifiers_.append(behavior_flag_modifier)

        # List of weeks on which we make checks
        self.checks_on_weeks = checks_on_weeks

        # We have a list of checks to run here
        self.checks = check_list()
        self.fill_check_list()

    def run(self):
        # To be done
        print("running check : %s" % self.__class__.__name__)
        self.checks.run()

    def fill_check_list(self):
        self.checks.add(check = check_logs_content(self.study_path,
                                                   self.new_behavior_flag.error_or_warning_keyword(),
                                                   self.checks_on_weeks))

        # self.checks.add(check = self.make_mps_check())

    def make_mps_check(self):
        pass
        """
        if self.new_behavior_flag.contains_verbose():
            return check_mps_existence(self.study_path, self.checks_on_weeks)
        else:
            return check_no_mps(self.study_path)
        """

