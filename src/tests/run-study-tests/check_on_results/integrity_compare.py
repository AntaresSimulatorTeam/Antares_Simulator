from pathlib import Path
import numpy

from utils.assertions import raise_assertion
from check_on_results.check_general import check_interface
from utils.find_reference import reference_folder_finder

class integrity_compare(check_interface):
    def __init__(self, study_path):
        check_interface.__init__(self, study_path)
        self.path_to_output = self.study_path / 'output'

        # Find reference folder
        self.ref_folder_finder = reference_folder_finder(study_path)
        if not self.ref_folder_finder.find():
            raise_assertion("Reference folder not found")
        self.ref_folder = self.ref_folder_finder.get()

    def need_output_results(self):
        return False

    def run(self):
        print("running check : %s" % self.__class__.__name__)
        self.check_integrity_first_opt()
        self.check_integrity_second_opt()

    def check_integrity_first_opt(self):
        reference_values = get_integrity_check_values(self.ref_folder)
        output_values = get_integrity_check_values(self.path_to_output)
        numpy.testing.assert_allclose(reference_values[0:4], output_values[0:4], rtol=1e-3, atol=0)

    def check_integrity_second_opt(self):
        reference_values = get_integrity_check_values(self.ref_folder)
        output_values = get_integrity_check_values(self.path_to_output)
        numpy.testing.assert_allclose(reference_values[4:8], output_values[4:8], rtol=1e-3, atol=0)


def get_integrity_check_values(output : Path) -> numpy.array :
    integrity_path = find_integrity_path(output)
    integrity_file = open(str(integrity_path), 'r')
    output_values = list()
    for x in integrity_file:
        output_values.append(float(x))
    assert len(output_values) == 8
    return output_values

def find_integrity_path(output_dir):
    op = []
    for path in Path(output_dir).rglob('checkIntegrity.txt'):
        op.append(path)
    assert len(op) == 1
    return op[0]