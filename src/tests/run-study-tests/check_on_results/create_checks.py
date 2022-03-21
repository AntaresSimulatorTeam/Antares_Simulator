from check_on_results.check_general import check_list
from check_on_results.output_compare import output_compare

def create_checks(study_path, checks_data = {}, simulation = None):
    checks = []
    for key, value in checks_data.items():
        if key == "output_compare":
            checks.append(create_output_compare(study_path, value))

    return checks

def create_output_compare(study_path, args_data = {}):
    arg_list = [study_path]
    for key, value in args_data:
        if value == "absolute_tolerances":
            continue
        if value == "relative_tolerances":
            continue
    return output_compare(*arg_list)