from pathlib import Path
import numpy

from check_on_results.check_general import check_interface
from check_decorators.print_name import printNameDecorator
from utils.find_reference import find_reference_folder
from utils.find_output import find_output_folder

@printNameDecorator
class integrity_compare(check_interface):
    def __init__(self, study_path):
        super().__init__(study_path)
        self.ref_folder = find_reference_folder(self.study_path)

    def run(self):
        self.compare_files()

    def name(self):
        return "integrity compare"

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
    files_found = []
    for path in Path(output_dir).rglob('checkIntegrity.txt'):
        files_found.append(path)
    assert len(files_found) == 1
    return files_found[0]