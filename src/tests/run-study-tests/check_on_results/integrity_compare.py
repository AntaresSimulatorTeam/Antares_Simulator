from pathlib import Path
import numpy

from utils.assertions import raise_assertion
from check_on_results.check_general import check_interface
from utils.find_reference import find_reference_folder
from utils.find_output import find_output_folder

class integrity_compare(check_interface):
    def __init__(self, study_path):
        check_interface.__init__(self, study_path)
        self.ref_folder = find_reference_folder(self.study_path)

    def need_output_results(self):
        return False

    def run(self):
        print("running check : %s" % self.__class__.__name__)
        self.compare_files()

    def compare_files(self):
        reference_values = get_integrity_check_values(self.ref_folder)

        path_to_output = find_output_folder(self.study_path)
        output_values = get_integrity_check_values(path_to_output)

        numpy.testing.assert_allclose(reference_values[0:8], output_values[0:8], rtol=1e-3, atol=0)


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