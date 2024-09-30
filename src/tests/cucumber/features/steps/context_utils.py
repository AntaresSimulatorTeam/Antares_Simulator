# Manage cached output data in "context" object

from output_utils import *

def get_annual_system_cost(context):
    if context.annual_system_cost is None:
        context.annual_system_cost = parse_annual_system_cost(context.output_path)
    return context.annual_system_cost

def get_values_hourly_for_specific_hour(context, area : str, year : int, date : str):
    df = get_values_hourly(context, area, year)
    day, month, hour = date.split(" ")
    return df.loc[(df['Unnamed: 2'] == int(day)) & (df['Unnamed: 3'] == month) & (df['Unnamed: 4'] == hour)]

def get_values_annual(context, area : str, year : int):
    if context.values_annual is None:
        context.values_annual = {}
    if area not in context.values_annual:
        context.values_annual[area] = {}
    if year not in context.values_annual[area]:
        context.values_annual[area][year] = parse_values_annual(context.output_path, area, year)
    return context.values_annual[area][year]

def get_values_hourly(context, area : str, year : int):
    if context.values_hourly is None:
        context.values_hourly = {}
    if area not in context.values_hourly:
        context.values_hourly[area] = {}
    if year not in context.values_hourly[area]:
        context.values_hourly[area][year] = parse_values_hourly(context.output_path, area, year)
    return context.values_hourly[area][year]

def get_details_hourly(context, area : str, year : int):
    if context.details_hourly is None:
        context.details_hourly = {}
    if area not in context.details_hourly:
        context.details_hourly[area] = {}
    if year not in context.details_hourly[area]:
        context.details_hourly[area][year] = parse_details_hourly(context.output_path, area, year)
    return context.details_hourly[area][year]