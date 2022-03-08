from study_input_handler import *

class print_results_handler:
    def __init__(self, study_path):
        self.study_path = study_path
        self.do_we_enable = False

        self.study_handler = study_input_handler(str(self.study_path))
        self.study_handler.check_files_existence()

    def results_already_enabled(self):
        if self.study_handler.get_value(variable = "synthesis", file_nick_name="general") == 'false':
            return False
        return True


    def enable_if_needed(self, check_list):
        self.do_we_enable = self.do_we_enable_results(check_list)
        print("Do we enable : %r" % self.do_we_enable)

        if self.do_we_enable:
            self.study_handler.set_value(variable = "synthesis", value = 'true', file_nick_name="general")

    def do_we_enable_results(self, check_list):
        # print()
        print("results already enabled : %r" % self.results_already_enabled())
        # print("output results needed : %r" % check_list.output_results_needed())
        return check_list.output_results_needed() and not self.results_already_enabled()

    def back_to_previous_state(self):
        if self.do_we_enable:
            self.study_handler.set_value(variable="synthesis", value='false', file_nick_name="general")