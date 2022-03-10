from os import walk
from pathlib import Path
from os.path import basename

def look_for_studies(root_folder):
    to_return = []
    for current_folder, sub_folders, files in walk(root_folder):
        if skip_folder(current_folder):
            sub_folders[:] = []

        if 'study.antares' in files:
            to_return.append(current_folder)
            continue

    return to_return

def skip_folder(folder):
    return basename(folder) in ['input', 'layers', 'settings', 'logs', 'output']

# Test on function "look_for_studies"
ALL_STUDIES_PATH = Path('../../resources/Antares_Simulator_Tests').resolve()
list_studies = look_for_studies(ALL_STUDIES_PATH)
for study_path in list_studies:
    print("%s" % study_path)

