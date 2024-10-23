# This file defines behave hooks to run before all tests

def before_all(context):
    check_userdata_exists(context, "antares-solver")
    check_userdata_exists(context, "resources-path")

def check_userdata_exists(context, data_name: str):
    if data_name not in context.config.userdata:
        raise Exception(
            f"The following parameter should be defined in 'behave.ini' or in the command line when calling behave: {data_name}")
