from os import walk
from pathlib import Path
from os.path import basename

def look_for_studies(root_folder):
    to_return = []
    for current_folder, sub_folders, files in walk(root_folder):
        if skip_folder(current_folder):
            sub_folders[:] = []

        if 'study.antares' in files:
            to_return.append(Path(current_folder))
            continue

    return to_return

def skip_folder(folder):
    return basename(folder) in ['input', 'layers', 'settings', 'logs', 'output']

