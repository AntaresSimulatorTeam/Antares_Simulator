# Antares modeler outputs parsing

import os
import pandas as pd
import configparser
from enum import Enum


class modeler_output_handler:

    def __init__(self, study_output_path):
        self.study_output_path = study_output_path
        self.results = self.__read_csv("solution.csv")


    def __read_csv(self, file_name) -> pd.DataFrame:
        absolute_path = os.path.join(self.study_output_path, file_name.replace("/", os.sep))
        return pd.read_csv(absolute_path, header=None, sep=' ', low_memory=False)

    def get_optimal_value(self, var : str) -> float:
        for row in self.results.iterrows():
            if row[1][0] == var:
                return row[1][1]
        raise ValueError("Variable not found")