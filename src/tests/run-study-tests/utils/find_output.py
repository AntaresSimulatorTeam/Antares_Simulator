from re import match
from os.path import isdir, basename
from os import walk
from pathlib import Path

from utils.assertions import check

class output_folder_finder:
    def __init__(self, study_path):
        self.output_folder = study_path / 'output'
        self.found_dir_path = None

    def find(self):
        for element in self.output_folder.iterdir():
            if isdir(element) and match('[0-9]{8}-[0-9]{4}', basename(element)):
                self.found_dir_path = element
                return True
        return False

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
    for root in Path(output_dir).iterdir():
        if root.stem in ["adequacy", "economy"]:
            return root
