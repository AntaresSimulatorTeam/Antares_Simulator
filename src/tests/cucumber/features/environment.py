# This file defines behave hooks to run before all tests
import os.path
from shutil import rmtree

def before_all(context):
    check_userdata_exists(context, "antares-solver")
    check_userdata_exists(context, "resources-path")

def check_userdata_exists(context, data_name: str):
    if data_name not in context.config.userdata:
        raise Exception(
            f"The following parameter should be defined in 'behave.ini' or in the command line when calling behave: {data_name}")

def after_scenario(context, scenario):
    # post-processing a test: remove tmp copy of study
    if hasattr(context, "tmp_workdir") and context.tmp_workdir is not None:
        rmtree(context.tmp_workdir)
        return
    # else, clean up output files to avoid taking up all the disk space
    if hasattr(context, "output_path") and context.output_path is not None:
        rmtree(context.output_path)
    if os.path.exists(os.path.join(context.study_path, "logs")):
        rmtree(os.path.join(context.study_path, "logs"))