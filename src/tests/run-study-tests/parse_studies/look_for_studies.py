import os.path
from os import walk
from pathlib import Path
from os.path import basename

def look_for_studies(root_folder, skipped_studies):
    to_return = []
    for current_folder, sub_folders, files in walk(root_folder):
        if skip_folder(current_folder):
            sub_folders[:] = []

        if 'study.antares' in files and not skip_study(current_folder, root_folder, skipped_studies):
            to_return.append(Path(current_folder))
            continue

    return sorted(to_return)

def skip_folder(folder):
    return basename(folder) in ['input', 'layers', 'settings', 'logs', 'output']

def skip_study(study, root_folder, skipped_studies):
    name = str(study).replace(str(root_folder) + os.path.sep, "")
    for skipped_study in skipped_studies:
        if skipped_study in name:
            return True
    return False

