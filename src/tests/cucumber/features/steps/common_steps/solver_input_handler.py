import os
from pathlib import Path
import shutil


class solver_input_handler:
    def __init__(self, study_root_directory):
        self.study_root_dir = study_root_directory
        self.name = os.path.basename(study_root_directory)
        self.files_path = {}
        self.files_path["desktop"] = self.study_root_dir / "Desktop.ini"
        self.files_path["general"] = self.study_root_dir / "settings" / "generaldata.ini"
        self.files_path["study"] = self.study_root_dir / "study.antares"
        self.files_path["thermal"] = self.study_root_dir / "study.antares"
        self.reference = self.study_root_dir / "output" / "reference"
        self.files_path["simulation_table1"] = self.reference / "simulation_table--optim-nb-1.csv"
        self.files_path["simulation_table2"] = self.reference / "simulation_table--optim-nb-2.csv"
        self.files_path["input"] = self.study_root_dir / "input"
        self.files_path["reserves"] = self.study_root_dir / "input" / "reserves"
        self.files_path["reserve_ini_folder"] = self.study_root_dir / ".." / "reserves_ini_files"

    def reference_dir(self):
        return self.reference

    def get_value(self, variable, file_nick_name):
        # File path
        file = self.files_path[file_nick_name]

        # Reading the file content (content in)
        with open(file) as f:
            # Searching variable and setting its value in a tmp content
            for line in f:
                if line.strip().startswith(variable):
                    return line.split('=')[1].strip()

    def get_input(self, input_file, section, variable):
        # File path
        file = self.study_root_dir / "input" / input_file.replace("/", os.sep)
        correct_section = False
        # Reading the file content (content in)
        with open(file) as f:
            # Searching variable and setting its value in a tmp content
            for line in f:
                if line.startswith("["):
                    if f"[{section}]" in line:
                        correct_section = True
                    else:
                        correct_section = False
                else:
                    if correct_section and line.strip().startswith(variable):
                        return line.split('=')[1].strip()

    def set_parameter_value(self, variable, value, file_nick_name: str):
        self.set_value(variable, value, self.files_path[file_nick_name])

    def set_value(self, variable, value, file_path: Path):
        # Content to print in file (tmp content)
        content_out = []
        # Reading the file content (content in)
        with open(file_path) as f:
            # Searching variable and setting its value in a tmp content
            for line in f:
                if line.strip().startswith(variable):
                    content_out.append(variable + " = " + value + "\n")
                else:
                    content_out.append(line)
        # Erasing file content with the tmp content (content out)
        with open(file_path, "w") as f:
            f.writelines(content_out)

    def get_optim1_simulation_table(self):
        assert self.files_path["simulation_table1"].exists(), f"Path %s does not exist." % self.files_path[
            "simulation_table1"]
        return open(self.files_path["simulation_table1"], 'r').readlines()

    def get_optim2_simulation_table(self):
        if self.files_path["simulation_table2"].exists():
            return open(self.files_path["simulation_table2"], 'r').readlines()
        else:
            return None

    def set_reserve_value(self, area, sectionName, variable, value):
        # File path
        file = self.files_path["reserves"] / area / "reserves.ini"
        # Content to print in file (tmp content)
        content_out = []
        # Reading the file content (content in)
        with open(file) as f:
            # Searching variable and setting its value in a tmp content
            for line in f:
                if line.startswith("["):
                    content_out.append(line)
                    if f"[{sectionName}]" in line:
                        correct_section = True
                    else:
                        correct_section = False
                else:
                    if correct_section and line.strip().startswith(variable):
                        content_out.append(variable + " = " + value + "\n")
                    else:
                        content_out.append(line)
        # Erasing file content with the tmp content (content out)
        with open(file, "w") as f:
            f.writelines(content_out)

    def copy_reserve_ini_from_file(self, origin, destination):
        # File path
        fileToReplace = os.path.join(self.study_root_dir, *destination)
        fileToCopy = os.path.join(self.study_root_dir, *origin)
        shutil.copyfile(fileToCopy, fileToReplace)

    def set_parameter_from_file(self, area, sectionName, paramName, paramValue):
        self.set_reserve_value(area.lower(), sectionName, paramName, paramValue)
