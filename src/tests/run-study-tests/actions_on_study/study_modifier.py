from actions_on_study.study_input_handler import study_input_handler

class study_modifier:

    def __init__(self, study_path, study_parameter, new_value, study_file_id):
        self.study_path = study_path
        self.parameter = study_parameter
        self.file_id = study_file_id
        self.new_value = new_value

        self.study_input_handler = study_input_handler(self.study_path)
        self.original_value = self.get_value()

    def get_value(self):
        return self.study_input_handler.get_value(variable=self.parameter, file_nick_name=self.file_id)

    def activate(self):
        self.study_input_handler.set_value(variable=self.parameter,
                                           value=self.new_value,
                                           file_nick_name=self.file_id)

    def rewind(self):
        self.study_input_handler.set_value(variable=self.parameter,
                                           value=self.original_value,
                                           file_nick_name=self.file_id)
