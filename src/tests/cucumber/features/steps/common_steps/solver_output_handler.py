# Antares outputs parsing

import os
import pandas as pd
import configparser
from enum import Enum
from pathlib import Path


class result_type(Enum):
    VALUES = "values"
    DETAILS = "details"
    DETAILS_STS = "details-STstorage"


class solver_output_handler:

    def __init__(self, study_output_path, mode):
        self.study_output_path = study_output_path
        self.mode = mode
        self.annual_system_cost = None
        # cache for hourly result tables
        self.hourly_results = {result_type.VALUES: None,
                               result_type.DETAILS: None,
                               result_type.DETAILS_STS: None}
        # cache for annual result tables
        self.annual_results = {}

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

    def __if_none_then_parse_annual(self, area: str, year: int, file_name: str) -> pd.DataFrame:
        # parse and cache annual result tables
        key_area = area.lower()
        if key_area not in self.annual_results:
            self.annual_results[key_area] = {}
        if year not in self.annual_results[key_area]:
            path = f"{self.mode}/mc-ind/{year:05d}/areas/{key_area}/{file_name}"
            self.annual_results[key_area][year] = self.__read_csv(path)
        return self.annual_results[key_area][year]

    def __get_values_annual(self, area: str, year: int) -> pd.DataFrame:
        """Internal parser for annual values, using caching"""
        return self.__if_none_then_parse_annual(area, year, "values-annual.txt")

    def __get_values_hourly(self, area: str, year: int) -> pd.DataFrame:
        return self.__if_none_then_parse(result_type.VALUES, area.lower(), year, "values-hourly.txt")

    def __get_values_hourly_for_specific_hour(self, area: str, year: int, date: str) -> pd.DataFrame:
        df = self.__get_values_hourly(area, year)
        return df.loc[df['datetime'] == date]

    def __get_values_hourly_for_specific_week(self, area: str, year: int, week: int) -> pd.DataFrame:
        df = self.__get_values_hourly(area, year)
        day_col = df['Unnamed: 2_level_0']
        week_calc = (day_col - 1) // 7 + 1
        return df.loc[week_calc == week]

    # common helper to get hourly series by header
    def _get_hourly_series(self, area: str, year: int, header: str) -> pd.Series:
        df = self.__get_values_hourly(area, year)
        return df.xs(header, axis=1, level=0).iloc[:, 0]

    # common helper to get annual series by header
    def _get_annual_series(self, area: str, year: int, header: str) -> pd.Series:
        df = self.__get_values_annual(area, year)
        return df.xs(header, axis=1, level=0).iloc[:, 0]

    # common helper to get annual scalar by header
    def _get_annual_scalar(self, area: str, year: int, header: str) -> float:
        return float(self._get_annual_series(area, year, header).iloc[0])

    def get_metric(self, area: str, year: int, header: str, period: str = 'hourly'):
        """
        Return a metric by header name: hourly Series if period='hourly', annual scalar if period='annual'.
        """
        if period == 'hourly':
            return self._get_hourly_series(area, year, header)
        if period == 'annual':
            return self._get_annual_scalar(area, year, header)
        raise ValueError(f"Unknown period '{period}', expected 'hourly' or 'annual'")

    def get_load(self, area: str, year: int, period: str = 'hourly'):
        """
        Return load for a given period ('hourly' returns a Series of hourly load,
        'annual' returns the annual scalar load).
        """
        header = 'LOAD'
        if period == 'hourly':
            return self._get_hourly_series(area, year, header)
        return self._get_annual_scalar(area, year, header)

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
        return int(self._get_hourly_series(area, year, 'SPIL. ENRG').sum())

    def get_hydro_production_mwh(self, area: str, year: int) -> int:
        return self.__get_values_hourly(area, year)["H. STOR"]["MWh"].sum()

    def get_hydro_pumping_mwh(self, area: str, year: int) -> int:
        return self.__get_values_hourly(area, year)["H. PUMP"]["MWh"].sum()

    def get_balance_mwh(self, area: str, year: int) -> int:
        return int(self._get_hourly_series(area, year, 'BALANCE').sum())

    # Add hourly series getters corresponding to annual metrics
    def get_hourly_margin_price(self, area: str, year: int) -> pd.Series:
        return self.get_metric(area, year, 'MRG. PRICE', period='hourly')

    def get_hourly_load(self, area: str, year: int) -> pd.Series:
        return self.get_metric(area, year, 'LOAD', period='hourly')

    def get_hourly_gas(self, area: str, year: int) -> pd.Series:
        return self.get_metric(area, year, 'GAS', period='hourly')

    def get_hourly_hard_coal(self, area: str, year: int) -> pd.Series:
        return self.get_metric(area, year, 'OTHER FUEL', period='hourly')

    def get_hourly_unsupplied_energy(self, area: str, year: int) -> pd.Series:
        return self.get_metric(area, year, 'UNSP. ENRG', period='hourly')

    def get_hourly_spilled_energy(self, area: str, year: int) -> pd.Series:
        return self.get_metric(area, year, 'SPIL. ENRG', period='hourly')

    def get_hourly_n_dispatched_units_total(self, area: str, year: int) -> pd.Series:
        return self.get_metric(area, year, 'NODU', period='hourly')

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

    def get_npcap_hours(self, area: str, year: int) -> int:
        # Return total NPCAP HOURS over hourly results
        return int(self.__get_values_hourly(area, year)["NPCAP HOURS"]["Hours"].sum())

    def get_npcap_hours_for_hour(self, area: str, year: int, hour: int) -> int:
        # Return NPCAP HOURS indicator at a specific hour (0-based index)
        df = self.__get_values_hourly(area, year)
        return int(df["NPCAP HOURS"]["Hours"].iloc[hour])

    def get_values_annual(self, area: str, year: int) -> pd.DataFrame:
        """Retourne le DataFrame des résultats annuels provenant de values-annual.txt"""
        return self.__get_values_annual(area, year)

    # Specific getters for annual variables
    def get_annual_margin_price(self, area: str, year: int) -> float:
        return self.get_metric(area, year, 'MRG. PRICE', period='annual')

    def get_annual_load(self, area: str, year: int) -> float:
        return self.get_metric(area, year, 'LOAD', period='annual')

    def get_annual_gas(self, area: str, year: int) -> float:
        return self.get_metric(area, year, 'GAS', period='annual')

    def get_annual_hard_coal(self, area: str, year: int) -> float:
        return self.get_metric(area, year, 'HARD COAL', period='annual')

    def get_annual_unsupplied_energy(self, area: str, year: int) -> float:
        return self.get_metric(area, year, 'UNSP. ENRG', period='annual')

    def get_annual_spilled_energy(self, area: str, year: int) -> float:
        return self.get_metric(area, year, 'SPIL. ENRG', period='annual')

    def get_annual_n_dispatched_units(self, area: str, year: int) -> int:
        return int(self.get_metric(area, year, 'NODU', period='annual'))
