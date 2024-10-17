from importlib import import_module
from pkgutil import iter_modules
from os import walk
from pathlib import Path


def import_steps(steps_dir: Path):
    assert steps_dir.exists()

    for (directory, _, _) in walk(steps_dir, followlinks=True):
        if "__pycache__" in directory:
            continue

        if directory == str(steps_dir):
            continue

        current_directory = directory + "/"
        print(f"Importing Additional Steps: {current_directory}")

        all_modules = [
            module_info[1]
            for module_info in iter_modules(path=[str(current_directory)])
        ]
        current_directory = current_directory.replace(str(steps_dir) + "/", "")

        for module in all_modules:
            module_path = current_directory.replace("/", ".") + module
            import_module(module_path)


def import_steps_from_subdirs(dir_path):
    for directory in walk(dir_path):
        current_directory = directory[0] + "/"

        all_modules = [
            module_info[1] for module_info in iter_modules(path=[current_directory])
        ]

        for module in all_modules:
            import_module(current_directory.replace("/", ".") + module)
