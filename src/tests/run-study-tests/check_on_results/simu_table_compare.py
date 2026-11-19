from pathlib import Path

from check_on_results.check_general import check_interface
from check_decorators.print_name import printNameDecorator
from utils.assertions import check
from utils.find_reference import find_reference_folder
from utils.find_output import find_dated_output_folder


@printNameDecorator
class simu_table_compare(check_interface):
    def __init__(self, study_path):
        super().__init__(study_path)
        self.ref_folder = find_reference_folder(self.study_path)

    def run(self):
        ref_simulation_folder = Path(self.ref_folder) / 'simulation-tables'
        path_to_output = find_dated_output_folder(self.study_path)
        simulation_folder = path_to_output / 'simulation-tables'

        reference_files = sorted(ref_simulation_folder.rglob("simulation-table-*-optim-nb-*.csv"))
        check(reference_files, f"No simulation table files found in {ref_simulation_folder}")

        for reference_file in reference_files:
            simulation_file = simulation_folder / reference_file.relative_to(ref_simulation_folder)
            check(simulation_file.is_file(), f"Missing simulation table file {simulation_file}")
            check(
                reference_file.read_bytes() == simulation_file.read_bytes(),
                f"Simulation table files differ: {reference_file} and {simulation_file}",
            )

    def name(self):
        return "simulation table compare"
