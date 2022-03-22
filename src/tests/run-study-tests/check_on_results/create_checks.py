from check_on_results.output_compare import output_compare
from check_on_results.integrity_compare import integrity_compare
from check_on_results.tolerances import get_tolerances


def create_checks(study_path, checks_data = {}, simulation = None):
    checks = []
    for check_type, check_params in checks_data.items():
        if check_type == "output_compare":
            checks.append(create_output_compare(study_path, check_params))
        if check_type == "integrity_compare":
            checks.append(create_intergrity_compare(study_path, check_params))

    return checks


def create_output_compare(study_path, parameters = {}):
    arg_list = [study_path]
    tolerances = get_tolerances()
    for tol_type, tol_content in parameters.items():
        if tol_type == "absolute_tolerances":
            tolerances.set_absolute(tol_content)
        if tol_content == "relative_tolerances":
            tolerances.set_relative(tol_content)
    arg_list.append(tolerances)
    return output_compare(*arg_list)

def create_intergrity_compare(study_path, parameters = {}):
    arg_list = [study_path]
    return integrity_compare(*arg_list)
