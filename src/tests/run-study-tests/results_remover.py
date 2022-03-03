import shutil

from os.path import isdir

class resuts_remover:
    def __init__(self, study_path):
        self.list_to_remove = []
        self.list_to_remove.append(study_path / 'output')
        self.list_to_remove.append(study_path / 'logs')

    def run(self):
        print("Removing results" )
        for dir in self.list_to_remove:
            if isdir(dir):
                shutil.rmtree(dir)