//
// Created by milos on 10/11/23.
//

#include "OptimizedGenerator.h"

namespace Antares::Solver::TSGenerator
{

void OptimizedThermalGenerator::GenerateOptimizedThermalTimeSeriesPerAllMaintenanceGroups()
{
    const auto& activeMaintenanceGroups = maintenanceGroupRepo.activeMaintenanceGroups();
    for (const auto& entryMaintenanceGroup : activeMaintenanceGroups)
    {
        auto& maintenanceGroup = *(entryMaintenanceGroup.get());
        calculateResidualLoad(maintenanceGroup);
        GenerateOptimizedThermalTimeSeriesPerOneMaintenanceGroup(maintenanceGroup);
    }
}

std::array<double, HOURS_PER_YEAR> OptimizedThermalGenerator::calculateAverageTs(
  const Matrix<double>& tsValue,
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

std::array<double, HOURS_PER_YEAR> OptimizedThermalGenerator::calculateAverageLoadTs(const Data::Area& area)
{
    // we assume ready-make TS - (pre-check exist for this!)
    const auto tsValues = area.load.series.timeSeries;
    const auto tsNumbers = area.load.series.timeseriesNumbers;
    return calculateAverageTs(tsValues, tsNumbers);
}

std::array<double, HOURS_PER_YEAR> OptimizedThermalGenerator::calculateAverageRorTs(
  const Data::Area& area)
{
    const auto tsValues = area.hydro.series->ror.timeSeries;
    auto tsNumbers = area.hydro.series->ror.timeseriesNumbers;
    return calculateAverageTs(tsValues, tsNumbers);
}

std::array<double, HOURS_PER_YEAR> OptimizedThermalGenerator::calculateAverageRenewableTsAggregated(
  const Data::Area& area)
{
    std::array<double, HOURS_PER_YEAR> averageTsSolar
      = calculateAverageTs(area.solar.series.timeSeries, area.solar.series.timeseriesNumbers);
    std::array<double, HOURS_PER_YEAR> averageTsWind
      = calculateAverageTs(area.wind.series.timeSeries, area.wind.series.timeseriesNumbers);

    std::array<double, HOURS_PER_YEAR> averageTsRenewable = {};
    for (std::size_t row = 0; row < HOURS_PER_YEAR; ++row)
    {
        averageTsRenewable[row] = (averageTsSolar[row] + averageTsWind[row]); // ask Hugo do we divide by 2 ?
    }
    return averageTsRenewable;
}

std::array<double, HOURS_PER_YEAR> OptimizedThermalGenerator::calculateAverageRenewableTsClusters(
  const Data::Area& area)
{
    std::array<double, HOURS_PER_YEAR> averageTsRenewable = {};
    for (const auto& entryCluster : area.renewable.clusters)
    {
        auto& cluster = *entryCluster;
        if (!cluster.enabled) // do we consider if cluster is of or on
            continue;
        auto tmpArrayPerCluster
          = calculateAverageTs(cluster.series.timeSeries, cluster.series.timeseriesNumbers);
        for (std::size_t row = 0; row < HOURS_PER_YEAR; ++row)
        {
            if (cluster.productionFactor)
                averageTsRenewable[row]
                  += tmpArrayPerCluster[row] * cluster.unitCount * cluster.nominalCapacity;
            else
                averageTsRenewable[row] += tmpArrayPerCluster[row];
        }
    }
    return averageTsRenewable;
}

std::array<double, HOURS_PER_YEAR> OptimizedThermalGenerator::calculateAverageRenewableTs(
  const Data::Area& area)
{
    if (study.parameters.renewableGeneration.isAggregated())
    {
        return calculateAverageRenewableTsAggregated(area);
    }
    else // clusters it is
    {
        return calculateAverageRenewableTsClusters(area);
    }
}

void OptimizedThermalGenerator::calculateResidualLoad(Data::MaintenanceGroup& maintenanceGroup)
{
    // create reference value arrays
    std::array<double, HOURS_PER_YEAR> refValueLoad = {};
    std::array<double, HOURS_PER_YEAR> refValueRor = {};
    std::array<double, HOURS_PER_YEAR> refValueRenewable = {};
    std::array<double, HOURS_PER_YEAR> refValue = {};

    // for phase II
    if (maintenanceGroup.type() == Data::MaintenanceGroup::typeTimeserie)
    {
        // read user defined ts - userProvidedResidualLoadTS_ with getter - phase-II
    }

    // loop through the elements of weightMap weights_
    for (const auto& entryWeightMap : maintenanceGroup)
    {
        const auto& area = *(entryWeightMap.first);
        const auto weights = (entryWeightMap.second);

        auto tmpLoad = calculateAverageLoadTs(area);
        auto tmpRor = calculateAverageRorTs(area);
        auto tmpRenewable = calculateAverageRenewableTs(area);

        for (std::size_t row = 0; row < HOURS_PER_YEAR; ++row)
        {
            refValueLoad[row] += tmpLoad[row] * weights.load;
            refValueRor[row] += tmpRor[row] * weights.ror;
            refValueRenewable[row] += tmpRenewable[row] * weights.renewable;
        }
    }
    // calculate reference value
    for (std::size_t row = 0; row < HOURS_PER_YEAR; ++row)
        refValue[row] = refValueLoad[row] - refValueRor[row] - refValueRenewable[row];
    // set ResidualLoadTS
    maintenanceGroup.setUsedResidualLoadTS(refValue);
}

void OptimizedThermalGenerator::GenerateOptimizedThermalTimeSeriesPerOneMaintenanceGroup(
  Data::MaintenanceGroup& maintenanceGroup)
{
    // loop through the elements of weightMap weights_
    for (const auto& entryWeightMap : maintenanceGroup)
    {
        const auto& area = *(entryWeightMap.first);
        // loop through the thermal clusters inside the area
        for (auto it = area.thermal.list.mapping.begin(); it != area.thermal.list.mapping.end();
             ++it)
        {
            auto& cluster = *(it->second);
            createOptimizationProblemPerCluster(area, cluster);
            ++pProgression;
        }
    }
}

void OptimizedThermalGenerator::createOptimizationProblemPerCluster(const Data::Area& area,
                                                                    Data::ThermalCluster& cluster)
{
    if (cluster.doWeGenerateTS(globalThermalTSgeneration_) && cluster.optimizeMaintenance)
    {
        // just playing here - this needs to go into new method - class  - operator
        logs.info() << "CR27-INFO: This cluster is active for mnt planning: "
                    << cluster.getFullName();

        if (!cluster.prepro)
        {
            logs.error() << "Cluster: " << area.name << '/' << cluster.name()
                         << ": The timeseries will not be regenerated. All data "
                            "related to the ts-generator for "
                         << "'thermal' have been released.";
        }

        assert(cluster.prepro);

        if (0 == cluster.unitCount || 0 == cluster.nominalCapacity || nbThermalTimeseries == 0)
        {
            cluster.series.timeSeries.reset(1, 8760);
        }
        else
        {
            cluster.series.timeSeries.reset(nbThermalTimeseries, 8760);
            cluster.series.timeSeries.fill(777.);
        }

        if (archive)
            writeResultsToDisk(area, cluster);

        // end playing
    }
}

} // namespace Antares::Solver::TSGenerator
