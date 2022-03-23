from pathlib import Path

from utils.csv import read_csv
from check_on_results.check_general import check_interface
from actions_on_study.study_modifier import study_modifier
from utils.assertions import check

class check_hydro_level(check_interface):
    def __init__(self, study_path, hour_in_year, level, tolerance):
        super().__init__(study_path)
        self.hour_in_year = hour_in_year
        self.level = level
        self.tol = tolerance

        print_results = study_modifier(self.study_path, study_parameter="synthesis", new_value="true",
                                       study_file_id="general")
        print_results.activate()
        self.study_modifiers_.append(print_results)

    @check_interface.print_check_name
    def run(self):
        reservoir_levels = fetch_hourly_values(self.study_path, area='area', year=1)["H. LEV"]
        err_msg = "Hydro level not correct : %.2f, expected : %.2f" % (reservoir_levels.iat[self.date_in_hours], self.level)
        check(abs(reservoir_levels.iat[self.date_in_hours] - self.level) < self.tol, err_msg)

    def check_name(self):
        return "hydro level"


def fetch_hourly_values(path, area, year):
    output_path = path / 'output'
    hourly_path = find_values_hourly_path(output_path, area, year)
    col_values = read_csv(hourly_path)
    # Check that file's number of lines is correct
    assert col_values.shape[0] == 364 * 24
    return col_values

def find_values_hourly_path(output_dir, area, year):
    op = []
    for path in Path(output_dir).rglob(f'mc-ind/{year:05d}/areas/{area}/values-hourly.txt'):
        op.append(path)
    assert len(op) == 1
    return op[0]