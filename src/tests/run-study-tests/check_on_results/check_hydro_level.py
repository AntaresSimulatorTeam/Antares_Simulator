from pathlib import Path

from utils.csv import read_csv
from check_on_results.check_general import check_interface
from utils.assertions import raise_assertion

class check_hydro_level(check_interface):
    def __init__(self, study_path, date_in_hours, level, tolerance):
        check_interface.__init__(self, study_path)
        self.date_in_hours = date_in_hours
        self.level = level
        self.tol = tolerance


    def need_output_results(self):
        return True

    def run(self):
        print("running check : %s" % self.__class__.__name__)
        reservoir_levels = fetch_hourly_values(self.study_path, area='area', year=1)["H. LEV"]
        if not abs(reservoir_levels.iat[self.date_in_hours] - self.level) < self.tol:
            raise_assertion("Hydro level not correct : %.2f" % reservoir_levels[self.date_in_hours])

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