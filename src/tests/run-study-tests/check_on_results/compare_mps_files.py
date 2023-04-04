from pathlib import Path

from check_decorators.print_name import printNameDecorator
from check_on_results.check_general import check_interface
from utils.find_reference import find_reference_folder
from utils.find_output import find_dated_output_folder, find_simulation_folder
from utils.assertions import check

@printNameDecorator
class compare_mps_files(check_interface):
    def __init__(self, study_path):
        super().__init__(study_path)
        self.study_path = study_path
        self.ref_folder = None
        self.dated_output_folder = None

    def find_folders_to_compare(self):
        # Retrieve the full path of 'reference' folder
        self.ref_folder = Path(find_reference_folder(self.study_path))

        # Folder of results (of which content is compared to content of reference folder)
        # ... of form yyyymmdd-hhmm<mode> (ex : 20230105-0944eco)
        self.dated_output_folder = Path(find_dated_output_folder(self.study_path))

    def run(self):
        self.find_folders_to_compare()

        ref_mps_files = list(self.ref_folder.glob('*.mps'))
        assert ref_mps_files

        mps_files = list(self.dated_output_folder.glob('**/*.mps'))
        assert mps_files

        list_of_pairs = [(mps_ref, mps) for mps_ref in ref_mps_files for mps in mps_files if mps_ref.name == mps.name]
        for pair in list_of_pairs:
            ref_content = open(pair[0]).read()
            output_content = open(pair[1]).read()
            check(ref_content == output_content, f"Difference between files {pair[0]} and {pair[1]}")

    def name(self):
        return "mps compare"