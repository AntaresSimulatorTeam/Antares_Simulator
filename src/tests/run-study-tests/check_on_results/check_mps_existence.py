from check_on_results.check_general import check_interface
from utils.assertions import raise_assertion

class check_mps_existence(check_interface):
    def __init__(self, study_path, checks_on_weeks):
        check_interface.__init__(self, study_path)
        self.checks_on_weeks = checks_on_weeks
        self.output_path = self.study_path / 'output'

    @check_interface.print_check_name
    def run(self):
        for item in self.checks_on_weeks:
            for week in item.weeks:
                mps_file_to_find = "problem-"+ str(item.year) + "-" + str(week)+ "-*-*.mps"
                if len(list(self.output_path.rglob(mps_file_to_find))) != 1:
                    raise_assertion("'" + mps_file_to_find + "'" + " not found in output directory")

    def check_name(self):
        return "mps existence"
