import shutil

from os import remove
from os.path import isdir, isfile, basename


class resuts_remover:
    def __init__(self, study_path):
        self.study_path = study_path

    def run(self):
        self.remove_logs()
        self.clean_results()

    def remove_logs(self):
        logs_folder = self.study_path / 'logs'
        if isdir(logs_folder):
            shutil.rmtree(logs_folder)

    def clean_results(self):
        output_folder = self.study_path / 'output'
        for element in output_folder.iterdir():
            if isdir(element) and basename(element) != 'reference':
                shutil.rmtree(element)
            if isfile(element):
                remove(element)

