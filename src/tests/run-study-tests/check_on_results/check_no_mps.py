from check_on_results.check_general import check_interface
from utils.assertions import raise_assertion

class check_no_mps(check_interface):
    def __init__(self, study_path):
        check_interface.__init__(self, study_path)
        self.output_path = self.study_path / 'output'

    def run(self):
        print("running check : %s" % self.__class__.__name__)
        if len(list(self.output_path.rglob("*.mps"))) != 0:
            raise_assertion("mps file found in output directory with 'dry' option")
