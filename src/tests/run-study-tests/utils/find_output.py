from re import match
from os.path import isdir, basename

from utils.assertions import raise_assertion

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
    if not output_folder_finder_.find():
        raise_assertion("Output folder not found")
    return output_folder_finder_.get()

"""
from pathlib import Path
STUDY_FOLDER = Path('d:/') / 'Users' / 'pierregui' / "Desktop" / "NTC" / "studies" / "NTC-2-areas"

found = find_output_folder(STUDY_FOLDER)
print("Found : %r" % found)
"""

