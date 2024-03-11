from check_on_results.check_general import check_list
from check_on_results.output_compare import output_compare
from check_on_results.integrity_compare import integrity_compare
from check_on_results.check_hydro_level import check_hydro_level
from check_on_results.unfeasible_problem import unfeasible_problem, weeks_in_year
from check_on_results.compare_mps_files import compare_mps_files
from check_on_results.compare_ts_files import compare_ts_files
from check_on_results.tolerances import get_tolerances


def create_checks(study_path, checks_data = {}, simulation = None):
    checks = check_list()
    if "output_compare" in checks_data:
        checks.add(create_output_compare(study_path, checks_data["output_compare"]))
    if "integrity_compare" in checks_data:
        checks.add(create_intergrity_compare(study_path))
    if "hydro_level" in checks_data:
        checks.add(create_check_hydro_level(study_path, checks_data["hydro_level"]))
    if "unfeasible_problem" in checks_data:
        checks.add(create_unfeasible_problem(study_path, checks_data["unfeasible_problem"], simulation))
    if "mps_compare" in checks_data:
        checks.add(create_mps_compare(study_path))
    if "ts_compare" in checks_data:
        checks.add(create_ts_compare(study_path))

    return checks

# --------------------------------------
# output_compare object creation
# --------------------------------------
def create_output_compare(study_path, parameters = {}):
    arg_list = [study_path]
    tolerances = get_tolerances()
    if "absolute_tolerances" in parameters:
        tolerances.set_absolute(parameters["absolute_tolerances"])
    if "relative_tolerances" in parameters:
        tolerances.set_relative(parameters["relative_tolerances"])
    arg_list.append(tolerances)
    return output_compare(*arg_list)


# --------------------------------------
# integrity_compare object creation
# --------------------------------------
def create_intergrity_compare(study_path):
    return integrity_compare(study_path)

# --------------------------------------
# check_hydro_level object creation
# --------------------------------------
def create_check_hydro_level(study_path, parameters = {}):
    return check_hydro_level(study_path,
                             parameters["hour_in_year"],
                             parameters["level"],
                             parameters["absolute_tolerance"])


# --------------------------------------
# unfeasible_problem object creation
# --------------------------------------
def create_unfeasible_problem(study_path, parameters = {}, simulation = None):
    behavior = parameters["behavior"]
    weeks = make_list_of_weeks(parameters["weeks_in_year"])
    return unfeasible_problem(study_path, behavior, weeks, simulation)

def make_list_of_weeks(weeks_in_year_data):
    checks_on_weeks = []
    for item in weeks_in_year_data:
        checks_on_weeks.append(weeks_in_year(year=item["year"], weeks=item["weeks"]))
    return checks_on_weeks

# --------------------------------------
# mps_compare object creation
# --------------------------------------
def create_mps_compare(study_path):
    return compare_mps_files(study_path)

# --------------------------------------
# ts_compare object creation
# --------------------------------------
def create_ts_compare(study_path):
    return compare_ts_files(study_path)
