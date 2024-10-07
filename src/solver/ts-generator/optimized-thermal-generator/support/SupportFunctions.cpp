//
// Created by milos on 14/11/23.
//
#include <array>
#include <numeric>
#include <algorithm>
#include "../main/OptimizedGenerator.h"
#include "../support/SupportFunctions.h"

namespace Antares::TSGenerator
{

// calculate Average time-series functions
std::array<double, HOURS_PER_YEAR> calculateAverageLoadTs(const Data::Area& area)
{
    // we assume ready-make TS - (pre-check exist for this!)
    const auto tsValues = area.load.series.timeSeries;
    const auto tsNumbers = area.load.series.timeseriesNumbers;
    return calculateAverageTs(tsValues, tsNumbers);
}

std::array<double, HOURS_PER_YEAR> calculateAverageRorTs(const Data::Area& area)
{
    const auto tsValues = area.hydro.series->ror.timeSeries;
    auto tsNumbers = area.hydro.series->ror.timeseriesNumbers;
    return calculateAverageTs(tsValues, tsNumbers);
}

std::array<double, HOURS_PER_YEAR> calculateAverageRenewableTsAggregated(const Data::Area& area)
{
    std::array<double, HOURS_PER_YEAR> averageTsSolar
      = calculateAverageTs(area.solar.series.timeSeries, area.solar.series.timeseriesNumbers);
    std::array<double, HOURS_PER_YEAR> averageTsWind
      = calculateAverageTs(area.wind.series.timeSeries, area.wind.series.timeseriesNumbers);

    std::array<double, HOURS_PER_YEAR> averageTsRenewable = {};
    for (std::size_t row = 0; row < HOURS_PER_YEAR; ++row)
    {
        averageTsRenewable[row] = (averageTsSolar[row] + averageTsWind[row]);
    }
    return averageTsRenewable;
}

std::array<double, HOURS_PER_YEAR> calculateAverageRenewableTsClusters(const Data::Area& area)
{
    std::array<double, HOURS_PER_YEAR> averageTsRenewable = {};
    for (const auto& entryCluster : area.renewable.list)
    {
        auto& cluster = *entryCluster;
        // this is not even necessary - because area.renewable.clusters returns list of only
        // ENABLED clusters but let's keep it for now
        if (!cluster.enabled)
            continue;
        auto tmpArrayPerCluster
          = calculateAverageTs(cluster.series.timeSeries, cluster.series.timeseriesNumbers);
        for (std::size_t row = 0; row < HOURS_PER_YEAR; ++row)
        {
            if (cluster.tsMode == Data::RenewableCluster::productionFactor)
                averageTsRenewable[row]
                  += tmpArrayPerCluster[row] * cluster.unitCount * cluster.nominalCapacity;
            else
                averageTsRenewable[row] += tmpArrayPerCluster[row];
        }
    }
    return averageTsRenewable;
}

std::array<double, HOURS_PER_YEAR> calculateAverageRenewableTs(
  const Data::Parameters::RenewableGeneration modelingType,
  const Data::Area& area)
{
    if (modelingType.isAggregated())
    {
        return calculateAverageRenewableTsAggregated(area);
    }
    else // clusters it is
    {
        return calculateAverageRenewableTsClusters(area);
    }
}

// support functions - for parameter calculations
std::array<double, DAYS_PER_YEAR> calculateDailySums(
  const std::array<double, HOURS_PER_YEAR>& hourlyValues)
{
    std::array<double, DAYS_PER_YEAR> dailyValues;
    auto hours_iter = hourlyValues.begin();

    for (double& day_sum : dailyValues)
    {
        day_sum = std::accumulate(hours_iter, hours_iter + 24, 0.0);
        hours_iter += 24;
    }

    return dailyValues;
}

std::array<double, HOURS_PER_YEAR> calculateAverageTs(const Matrix<double>& tsValue,
                                                      const Matrix<uint32_t>& tsNumbers)
{
    // define array
    std::array<double, HOURS_PER_YEAR> averageTs = {};
    // calculate sum
    for (std::size_t year = 0; year < tsNumbers.height; ++year)
    {
        for (std::size_t row = 0; row < HOURS_PER_YEAR; ++row)
        {
            averageTs[row] += tsValue[tsNumbers[0][year]][row];
        }
    }
    // calculate mean
    for (std::size_t row = 0; row < HOURS_PER_YEAR; ++row)
    {
        averageTs[row] = averageTs[row] / tsNumbers.height;
    }
    // return
    return averageTs;
}

bool checkClusterExist(const Data::ThermalCluster& cluster)
{
    if (!cluster.prepro)
    {
        logs.error() << "Cluster: " << cluster.getFullName()
                     << ": The timeseries will not be regenerated. All data "
                        "related to the ts-generator for "
                     << "'thermal' have been released.";
        return false;
    }

    if (0 == cluster.unitCount || 0 == cluster.nominalCapacity)
    {
        return false;
    }
    return true;
}

int dayOfTheYear(int optimizationDay)
{
    return optimizationDay % DAYS_PER_YEAR;
}

// calculate parameters methods - per cluster
int calculateAverageMaintenanceDuration(const Data::ThermalCluster& cluster)
{
    double sum = 0.0;
    for (std::size_t row = 0; row < DAYS_PER_YEAR; ++row)
    {
        sum += cluster.prepro->data[Data::PreproThermal::poDuration][row];
    }
    // poDuration in Antares cannot be below 1.0
    // so it is redundant to check here if return value is above 1.0
    // that is why I did not use std::max()
    return sum / static_cast<double>(DAYS_PER_YEAR);
}

std::array<double, DAYS_PER_YEAR> calculateMaxUnitOutput(const Data::ThermalCluster& cluster)
{
    std::array<double, DAYS_PER_YEAR> maxOutputDailyValues = {};
    std::array<double, HOURS_PER_YEAR> maxOutputHourlyValues = {};

    // transfer to array
    for (std::size_t row = 0; row < HOURS_PER_YEAR; ++row)
    {
        maxOutputHourlyValues[row]
          = cluster.modulation[Data::ThermalModulation::thermalModulationCapacity][row];
    }

    maxOutputDailyValues = calculateDailySums(maxOutputHourlyValues);
    // multiply by per unit power (nominal capacity)
    for (double& num : maxOutputDailyValues)
    {
        num *= cluster.nominalCapacity;
    }
    return maxOutputDailyValues;
}

std::array<double, DAYS_PER_YEAR> calculateAvrUnitDailyCost(const Data::ThermalCluster& cluster)
{
    std::array<double, DAYS_PER_YEAR> avrCostDailyValues = {};
    std::array<double, HOURS_PER_YEAR> costHourlyValues = {};

    // transfer to array
    for (std::size_t row = 0; row < HOURS_PER_YEAR; ++row)
    {
        costHourlyValues[row]
          = cluster.modulation[Data::ThermalModulation::thermalModulationMarketBid][row];
    }

    avrCostDailyValues = calculateDailySums(costHourlyValues);
    // multiply by per unit/cluster market bid cost + average this on 24 hours
    for (double& num : avrCostDailyValues)
    {
        num *= cluster.marketBidCost / 24.0;
    }
    return avrCostDailyValues;
}

} // namespace Antares::TSGenerator
