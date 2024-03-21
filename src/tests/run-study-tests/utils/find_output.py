from re import match
import pathlib, os

from utils.assertions import check

class output_folder_finder:
    def __init__(self, study_path):
        self.output_folder = study_path / 'output'
        self.found_dir_path = None

    def find(self):
        dirs = pathlib.Path(self.output_folder).glob('*/')
        self.found_dir_path = max(dirs, key=os.path.getmtime)
        return True

    def get(self):
        return self.found_dir_path

def find_dated_output_folder(study_path):
    # From the study root folder, search and return full path of the simulation folder,
    # which is of form : yyyymmdd-hhmm<mode> (ex : 20230105-0944eco)
    output_folder_finder_ = output_folder_finder(study_path)
    check(output_folder_finder_.find(), "Output folder not found")
    return output_folder_finder_.get()

def find_simulation_folder(output_dir):
    # Return full path of the ouptut simulation path (can be "adequacy", "economy")
    for root, dirs, files in os.walk(output_dir):
        if os.path.basename(root) in ["adequacy", "economy"]:
            return pathlib.Path(root)

