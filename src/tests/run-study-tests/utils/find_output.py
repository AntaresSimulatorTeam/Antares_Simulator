from re import match
from os.path import isdir, basename

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

def find_output_folder(study_path):
    output_folder_finder_ = output_folder_finder(study_path)
    check(output_folder_finder_.find(), "Output folder not found")
    return output_folder_finder_.get()

