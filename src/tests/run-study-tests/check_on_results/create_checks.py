from check_on_results.output_compare import output_compare
from check_on_results.integrity_compare import integrity_compare
from check_on_results.check_hydro_level import check_hydro_level
from check_on_results.tolerances import get_tolerances


def create_checks(study_path, checks_data = {}, simulation = None):
    checks = []
    if "output_compare" in checks_data:
        checks.append(create_output_compare(study_path, checks_data["output_compare"]))
    if "integrity_compare" in checks_data:
        checks.append(create_intergrity_compare(study_path, checks_data["integrity_compare"]))
    if "hydro_level" in checks_data:
        checks.append(create_check_hydro_level(study_path, checks_data["hydro_level"]))


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
def create_intergrity_compare(study_path, parameters = {}):
    arg_list = [study_path]
    return integrity_compare(*arg_list)

# --------------------------------------
# check_hydro_level object creation
# --------------------------------------
def create_check_hydro_level(study_path, parameters = {}):
    arg_list = [study_path]
    if "hour_in_year" in parameters:
        arg_list.append(parameters["hour_in_year"])
    if "absolute_tolerance" in parameters:
        arg_list.append(parameters["absolute_tolerance"])
    if "level" in parameters:
        arg_list.append(parameters["level"])
    return check_hydro_level(*arg_list)
