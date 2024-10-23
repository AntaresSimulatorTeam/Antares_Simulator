from importlib import import_module
from pkgutil import iter_modules
from os import walk
from pathlib import Path

import os


def import_steps(steps_dir: Path):
    assert steps_dir.exists()

    for (directory, _, _) in walk(steps_dir, followlinks=True):
        if "__pycache__" in directory:
            continue

        if directory == str(steps_dir):
            continue

        current_directory = directory + os.sep
        print(f"Importing Additional Steps: {current_directory}")

        all_modules = [
            module_info[1]
            for module_info in iter_modules(path=[str(current_directory)])
        ]
        current_directory = current_directory.replace(str(steps_dir) + os.sep, "")

        for module in all_modules:
            module_path = current_directory.replace(os.sep, ".") + module
            import_module(module_path)
