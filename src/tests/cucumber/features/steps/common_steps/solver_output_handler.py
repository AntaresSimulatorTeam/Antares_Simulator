# Antares outputs parsing

import os
import pandas as pd
import configparser
from enum import Enum
from pathlib import Path

from numpy.ma.core import absolute


class result_type(Enum):
    VALUES = "values"
    DETAILS = "details"
    DETAILS_STS = "details-STstorage"

class solver_output_handler:

    def __init__(self, study_output_path, mode):
        self.study_output_path = study_output_path
        self.mode = mode
        self.annual_system_cost = None
        self.hourly_results = {result_type.VALUES: None,
                               result_type.DETAILS: None,
                               result_type.DETAILS_STS: None}

    def get_annual_system_cost(self):
        if self.annual_system_cost is None:
            self.__parse_annual_system_cost()
        return self.annual_system_cost

    def __parse_annual_system_cost(self):
        file = open(os.path.join(self.study_output_path, "annualSystemCost.txt"), 'r')
        keys = ["EXP", "STD", "MIN", "MAX"]
        annual_system_cost = {}
        for line in file.readlines():
            for key in keys:
                if key in line:
                    annual_system_cost[key] = float(line.split(key + " : ")[1])
        self.annual_system_cost = annual_system_cost

    def get_simu_time(self) -> float:
        execution_info = configparser.ConfigParser()
        execution_info.read(os.path.join(self.study_output_path, "execution_info.ini"))
        return float(execution_info['durations_ms']['total']) / 1000

    def get_optim1_simulation_table(self):
        absolute_path = Path(os.path.join(self.study_output_path, "simulation_table--optim-nb-1.csv"))
        assert absolute_path.exists(), f"Path %s does not exist." % absolute_path
        return open(absolute_path, 'r').readlines()

    def get_optim2_simulation_table(self):
        absolute_path = Path(os.path.join(self.study_output_path, "simulation_table--optim-nb-2.csv"))
        if absolute_path.exists():
            return open(absolute_path, 'r').readlines()
        else:
            return None

    def __read_csv(self, file_name) -> pd.DataFrame:
        ignore_rows = [0, 1, 2, 3, 6]
        absolute_path = Path(os.path.join(self.study_output_path, file_name.replace("/", os.sep)))
        assert absolute_path.exists(), f"Path %s does not exist." % absolute_path
        return pd.read_csv(absolute_path, header=[0, 1], skiprows=ignore_rows, sep='\t', low_memory=False)

    def __if_none_then_parse(self, rs: result_type, area, year, file_name: str):
        if self.hourly_results[rs] is None:
            self.hourly_results[rs] = {}
        if area not in self.hourly_results[rs]:
            self.hourly_results[rs][area] = {}
        if year not in self.hourly_results[rs][area]:
            # parse file
            self.hourly_results[rs][area][year] = self.__read_csv(
                f"{self.mode}/mc-ind/{year:05d}/areas/{area}/{file_name}")
            # add datetime column by concatenating unnamed columns 2 (day), 3 (month), 4 (hour)
            cols = ['Unnamed: 2_level_0', 'Unnamed: 3_level_0', 'Unnamed: 4_level_0']
            self.hourly_results[rs][area][year]["datetime"] = self.hourly_results[rs][area][year][cols].apply(
                lambda row: ' '.join(row.values.astype(str)), axis=1)
        return self.hourly_results[rs][area][year]

    def __get_values_hourly(self, area: str, year: int):
        return self.__if_none_then_parse(result_type.VALUES, area.lower(), year, "values-hourly.txt")

    def __get_values_hourly_for_specific_week(self, area: str, year: int, week: int):
        df = self.__if_none_then_parse(result_type.VALUES, area.lower(), year, "values-hourly.txt")
        return df[(df['hourly']['Unnamed: 1_level_1'] > (week - 1) * 168) & (df['hourly']['Unnamed: 1_level_1'] <= week * 168)]

    def __get_values_hourly_for_specific_hour(self, area: str, year: int, datetime: str):
        df = self.__get_values_hourly(area, year)
        return df.loc[df['datetime'] == datetime]

    def __get_sts_details_hourly(self, area: str, year: int):
        return self.__if_none_then_parse(result_type.DETAILS_STS, area.lower(), year, "details-STstorage-hourly.txt")

    def details_hourly_for_sts(self, area: str, year: int):
        return self.__get_sts_details_hourly(area, year)

    def injection_for_sts(self, area: str, year: int, sts: str):
        return self.details_hourly_for_sts(area, year)[sts]['P-injection - MW']

    def withdrawal_for_sts(self, area: str, year: int, sts: str):
        return self.details_hourly_for_sts(area, year)[sts]['P-withdrawal - MW']

    def level_for_sts(self, area: str, year: int, sts: str):
        return self.details_hourly_for_sts(area, year)[sts]['Levels - MWh']

    def __get_details_hourly(self, area: str, year: int):
        return self.__if_none_then_parse(result_type.DETAILS, area.lower(), year, "details-hourly.txt")

    def details_hourly_for_cluster(self, area: str, year: int, cluster: str):
        return self.__if_none_then_parse(result_type.DETAILS, area.lower(), year, "details-hourly.txt")[cluster]

    def get_hourly_prod_mwh(self, area: str, year: int, prod_name: str) -> pd.Series:
        return self.__get_details_hourly(area, year)[prod_name]['MWh']

    def get_hourly_n_dispatched_units(self, area: str, year: int, prod_name: str) -> pd.Series:
        return self.__get_details_hourly(area, year)[prod_name]['NODU']

    def get_loss_of_load_weekly_duration_h(self, area: str, year: int, week: int) -> int:
        df = self.__get_values_hourly_for_specific_week(area, year, week)
        return self.__get_values_hourly_for_specific_week(area, year, week)["LOLD"]["Hours"].sum()

    def get_loss_of_load_duration_h(self, area: str, year: int) -> int:
        return self.__get_values_hourly(area, year)["LOLD"]["Hours"].sum()

    def get_spilled_energy_mwh(self, area: str, year: int) -> int:
        return self.__get_values_hourly(area, year)["SPIL. ENRG"]["MWh"].sum()

    def get_hydro_production_mwh(self, area: str, year: int) -> int:
        return self.__get_values_hourly(area, year)["H. STOR"]["MWh"].sum()

    def get_hydro_pumping_mwh(self, area: str, year: int) -> int:
        return self.__get_values_hourly(area, year)["H. PUMP"]["MWh"].sum()

    def get_balance_mwh(self, area: str, year: int) -> int:
        return self.__get_values_hourly(area, year)["BALANCE"]["MWh"].sum()

    def get_unsupplied_energy_mwh(self, area: str, year: int, date: str = None) -> float:
        if date is None:
            return self.__get_values_hourly(area, year)["UNSP. ENRG"]["MWh"].sum()
        else:
            return self.__get_values_hourly_for_specific_hour(area, year, date)["UNSP. ENRG"]["MWh"].sum()

    def min_gen_for_thermal_cluster(self, area: str, year: int, cluster: str):
        return self.__get_details_hourly(area, year)[cluster]["MIN GEN - MWh"]

    def min_gen_for_thermal_cluster_at_hour(self, area: str, year: int, hour: int, cluster_name: str):
        return self.__get_details_hourly(area, year)[cluster_name]["MIN GEN - MWh"][hour]

    def get_specific_value(self, area: str, year: int, prod_name: str, date: str) -> float:
        return self.__get_values_hourly_for_specific_hour(area, year, date)[prod_name]["MWh"].sum()

    def get_non_proportional_cost(self, area: str, year: int) -> float:
        return self.__get_values_hourly(area, year)["NP COST"]["Euro"].sum()
