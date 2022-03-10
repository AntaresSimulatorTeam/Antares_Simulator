from os import walk
from os.path import basename

class reference_folder_finder:
    def __init__(self, study_path):
        self.study_path = study_path
        self.found_dir_path = None
        self.skipped_folders_when_searching = ['input', 'layers', 'settings', 'logs']

    def find(self):
        for current_folder, sub_folders, files in walk(self.study_path):
            if basename(current_folder) == "reference" :
                self.found_dir_path = current_folder
                return True

            if self.skip_folder(current_folder):
                continue

        return False

    def get(self):
        return self.found_dir_path


    def skip_folder(self, folder):
        return basename(folder) in self.skipped_folders_when_searching
