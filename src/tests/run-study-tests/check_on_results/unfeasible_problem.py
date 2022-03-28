
from check_on_results.check_general import check_interface, check_list
from check_on_results.check_logs_content import check_logs_content
from check_on_results.sim_return_code import simulation_success
from actions_on_study.study_modifier import study_modifier
from utils.assertions import check


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
        error_message = "behavior incorrect : %s" % self.raw_behavior
        check(len(self.split_behavior) == 2, error_message)
        check(self.split_behavior[0] in ['error', 'warning'], error_message)
        check(self.split_behavior[1] in ['dry', 'verbose'], error_message)

    def error_or_warning_keyword(self):
        return self.error_or_warning_converter[self.split_behavior[0]]

    def contains_warning(self):
        if self.split_behavior[0] == 'warning':
            return True
        return False

    def contains_verbose(self):
        if self.split_behavior[1] == 'verbose':
            return True
        return False


class unfeasible_problem(check_interface):
    def __init__(self, study_path, new_behavior="error-verbose", checks_on_weeks = [], simulation = None):
        super().__init__(study_path)

        # Unfeasible problem behavior flag
        self.new_behavior_flag = behavior_flag(new_behavior)
        self.new_behavior_flag.check_conformity()
        behavior_flag_modifier = study_modifier(self.study_path, study_parameter="include-unfeasible-problem-behavior",
                                                new_value=new_behavior, study_file_id="general")
        behavior_flag_modifier.activate()
        self.study_modifiers_.append(behavior_flag_modifier)

        # List of weeks on which we make checks
        self.checks_on_weeks = checks_on_weeks

        # Store simulation
        self.simulation = simulation

        # We have a list of checks to run here
        self.checks = check_list()
        self.fill_check_list()

    @check_interface.print_check_name
    def run(self):
        self.checks.run()

    def check_name(self):
        return "unfeasible problem"

    def fill_check_list(self):
        self.checks.add(check = check_logs_content(self.study_path,
                                                   self.new_behavior_flag.error_or_warning_keyword(),
                                                   self.checks_on_weeks))

        self.checks.add(check = simulation_success(self.study_path,
                                                   simulation=self.simulation,
                                                   success_expected=self.new_behavior_flag.contains_warning()))

        # self.checks.add(check = self.make_mps_check())

    def make_mps_check(self):
        pass
        """
        if self.new_behavior_flag.contains_verbose():
            return check_mps_existence(self.study_path, self.checks_on_weeks)
        else:
            return check_no_mps(self.study_path)
        """

