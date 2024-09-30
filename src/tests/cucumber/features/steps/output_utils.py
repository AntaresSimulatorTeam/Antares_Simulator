# Antares outputs parsing

import os
import pandas
import configparser


def parse_output_folder_from_logs(logs: bytes) -> str:
    for line in logs.splitlines():
        if b'Output folder : ' in line:
            return line.split(b'Output folder : ')[1].decode('ascii')
    raise LookupError("Could not parse output folder in output logs")


def parse_annual_system_cost(output_path: str) -> dict:
    file = open(os.path.join(output_path, "annualSystemCost.txt"), 'r')
    keys = ["EXP", "STD", "MIN", "MAX"]
    annual_system_cost = {}
    for line in file.readlines():
        for key in keys:
            if key in line:
                annual_system_cost[key] = float(line.split(key + " : ")[1])
    return annual_system_cost


def parse_simu_time(output_path: str) -> float:
    execution_info = configparser.ConfigParser()
    execution_info.read(os.path.join(output_path, "execution_info.ini"))
    return float(execution_info['durations_ms']['total']) / 1000


def parse_values_hourly(output_path: str, area: str, year: int):
    return read_csv(os.path.join(output_path, "economy", "mc-ind", f"{year:05d}", "areas", area, "values-hourly.txt"))


def parse_details_hourly(output_path: str, area: str, year: int):
    return read_csv(os.path.join(output_path, "economy", "mc-ind", f"{year:05d}", "areas", area, "details-hourly.txt"))


def read_csv(file_name):
    ignore_rows = [0, 1, 2, 3, 5, 6]
    return pandas.read_csv(file_name, skiprows=ignore_rows, sep='\t', low_memory=False)


def compute_min_up_and_down_durations(prod):
    min_up = 1e10
    min_down = 1e10
    up = 1 if prod[0] > 1e-6 else 0
    down = 0 if prod[0] > 1e-6 else 1
    for t in range(1, len(prod)):
        if prod[t] > 1e-6:
            up += 1
            if prod[t - 1] < 1e-6:
                min_down = min(min_down, down)
                down = 0
        else:
            down += 1
            if prod[t - 1] > 1e-6:
                min_up = min(min_up, up)
                up = 0
    return min_up, min_down
