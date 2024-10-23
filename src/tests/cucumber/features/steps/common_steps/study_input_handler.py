import os


class study_input_handler:
    def __init__(self, study_root_directory):
        self.study_root_dir = study_root_directory
        self.name = os.path.basename(study_root_directory)
        self.files_path = {}
        self.files_path["desktop"] = self.study_root_dir / "Desktop.ini"
        self.files_path["general"] = self.study_root_dir / "settings" / "generaldata.ini"
        self.files_path["study"] = self.study_root_dir / "study.antares"
        self.files_path["thermal"] = self.study_root_dir / "study.antares"

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

    def set_value(self, variable, value, file_nick_name):
        # File path
        file = self.files_path[file_nick_name]
        # Content to print in file (tmp content)
        content_out = []
        # Reading the file content (content in)
        with open(file) as f:
            # Searching variable and setting its value in a tmp content
            for line in f:
                if line.strip().startswith(variable):
                    content_out.append(variable + " = " + value + "\n")
                else:
                    content_out.append(line)
        # Erasing file content with the tmp content (content out)
        with open(file, "w") as f:
            f.writelines(content_out)
