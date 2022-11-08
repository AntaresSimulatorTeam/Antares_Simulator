import pandas
from os import walk, sep
from os.path import basename
from pathlib import Path
from check_on_results.tolerances import get_tolerances

from check_on_results.check_general import check_interface
from check_decorators.print_name import printNameDecorator
from utils.assertions import check
from utils.find_reference import find_reference_folder
from utils.find_output import find_output_folder
from actions_on_study.study_modifier import study_modifier
from utils.csv import read_csv


@printNameDecorator
class output_compare(check_interface):
    def __init__(self, study_path, tolerances = get_tolerances()):
        super().__init__(study_path)
        self.tol = tolerances
        self.ref_folder = find_reference_folder(self.study_path)

        print_results = study_modifier(self.study_path, study_parameter="synthesis", new_value="true",
                                       study_file_id="general")
        print_results.activate()
        self.study_modifiers_.append(print_results)

    def run(self):
        reference_folder = find_simulation_folder(self.ref_folder)

        path_to_output = find_output_folder(self.study_path)
        other_folder = find_simulation_folder(path_to_output)

        simulation_files = find_simulation_files(reference_folder, other_folder)

        check(compare_simulation_files(simulation_files, self.tol), "Results comparison failed")

    def name(self):
        return "output compare"


def find_simulation_folder(output_dir):
    for root, dirs, files in walk(output_dir):
        if basename(root) in ["adequacy", "economy", "adequacy-draft"]:
            return Path(root)

def find_simulation_files(reference_folder, other_folder):
    list_files_to_compare = []

    # Walk through (or traverse) the reference folder
    for current_folder, sub_folders, files in walk(reference_folder):
        if skip_folder(current_folder):
            continue

        for file in files:
            if skip_file(file):
                continue
            current_file_path = Path(current_folder) / file
            list_files_to_compare.append((current_file_path, other_folder / current_file_path.relative_to(reference_folder)))

    return list_files_to_compare

def skip_folder(folder):
    return basename(folder) in ['grid']

def skip_file(file):
    id_files = ['id-hourly.txt',
                'id-daily.txt',
                'id-weekly.txt',
                'id-monthly.txt',
                'id-annual.txt']

    bc_files = ['binding-constraints-hourly.txt',
                'binding-constraints-daily.txt',
                'binding-constraints-weekly.txt',
                'binding-constraints-monthly.txt',
                'binding-constraints-annual.txt']

    return (file in id_files) or (file in bc_files)

def compare_simulation_files(simulation_files, tol):
    REF_INDEX = 0
    OTHER_INDEX = 1
    at_least_one_diff = False
    for file_pair in simulation_files:
        # Read reference and simulation (other) files
        ref_data_frame = read_csv(file_pair[REF_INDEX])
        other_data_frame = read_csv(file_pair[OTHER_INDEX])

        # Check that reference column titles are a subset of the simulation titles
        ref_column_titles = get_headers(ref_data_frame)
        other_column_titles = get_headers(other_data_frame)

        check(ref_column_titles.issubset(other_column_titles),
              f"The following column(s) is missing in the reference {ref_column_titles.difference(other_column_titles)}")

        for col_name in ref_column_titles:
            try:
                pandas.testing.assert_series_equal(ref_data_frame[col_name], other_data_frame[col_name], \
                                               atol=tol.absolute(col_name), rtol=tol.relative(col_name))
            except AssertionError:  # Catch and re-raise exception to print col_name & tolerances
                # Reduced path of files we're comparing
                file_path = sep.join(file_pair[REF_INDEX].absolute().parts[-4:])

                print_comparison_report(ref_data_frame, other_data_frame, file_path, col_name)
                at_least_one_diff = True

    return not at_least_one_diff


def print_comparison_report(ref_data_frame, other_data_frame, file_path, col_name):
    # Max number of rows to be printed in the report
    pandas.set_option("display.max_rows", 10)

    # At this stage, report contains only 2 comulms with lines that differ
    comparison_report = ref_data_frame[col_name].compare(other_data_frame[col_name])

    # Renaming columns
    comparison_report = comparison_report.rename(columns={'self': 'reference', 'other': 'results'})

    # Print title of the comparison report
    print("\n%s : %s" % (file_path, col_name))

    # Add a column that contains the differences
    comparison_report['diff'] = comparison_report['results'] - comparison_report['reference']
    print(comparison_report)

def get_headers(df) -> set :
    return set(df.columns)




