from check_on_results.check_general import check_interface
from check_decorators.print_name import printNameDecorator
from utils.assertions import check, raise_assertion
from utils.find_output import find_output_folder

@printNameDecorator
class check_logs_content(check_interface):
    def __init__(self, study_path, log_level="warns", faulty_weeks=[]):
        super().__init__(study_path)
        self.log_level = log_level
        self.expected_faulty_weeks = faulty_weeks

        check(log_level in ["warns", "fatal"], "Unknown log level : %s" % log_level)

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

        # Search for expected lines in log file content and store matches
        log_file = open(str(path_to_log_file), 'r')
        for line in log_file:
            for i in range(len(lines_to_find)):
                if lines_to_find[i] in line:
                    match_lines[i] = True

        # Report of lines not found
        for i in range(len(lines_to_find)):
            assert match_lines[i], "'" + lines_to_find[i] + "'" + " log not found in simulation.log"

    def name(self):
        return "unfeasible problem logs"


def find_logs_file(path_to_output_folder):
    found_files = []
    for path in path_to_output_folder.rglob('simulation.log'):
        found_files.append(path)
    if len(found_files) == 0:
        raise_assertion("Found no log file for simulation")
    elif len(found_files) > 1:
        raise_assertion("Found too many log files for simulation")
    return found_files[0]