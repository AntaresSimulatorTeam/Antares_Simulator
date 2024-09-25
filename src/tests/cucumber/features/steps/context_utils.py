# Manage cached output data in "context" object

from output_utils import *

def get_annual_system_cost(context):
    if context.annual_system_cost is None:
        context.annual_system_cost = parse_annual_system_cost(context.output_path)
    return context.annual_system_cost

def get_hourly_values_for_specific_hour(context, area : str, year : int, date : str):
    df = get_hourly_values(context, area, year)
    day, month, hour = date.split(" ")
    return df.loc[(df['Unnamed: 2'] == int(day)) & (df['Unnamed: 3'] == month) & (df['Unnamed: 4'] == hour)]

def get_hourly_values(context, area : str, year : int):
    if context.hourly_values is None:
        context.hourly_values = {}
    if area not in context.hourly_values:
        context.hourly_values[area] = {}
    if year not in context.hourly_values[area]:
        context.hourly_values[area][year] = parse_hourly_values(context.output_path, area, year)
    return context.hourly_values[area][year]