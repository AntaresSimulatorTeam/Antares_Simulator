from check_on_results.check_general import check_interface
from utils.assertions import raise_assertion

class check_no_mps(check_interface):
    def __init__(self, study_path):
        check_interface.__init__(self, study_path)
        self.output_path = self.study_path / 'output'

    @check_interface.print_name
    def run(self):
        if len(list(self.output_path.rglob("*.mps"))) != 0:
            raise_assertion("mps file found in output directory with 'dry' option")

    def name(self):
        return "no mps generated"
