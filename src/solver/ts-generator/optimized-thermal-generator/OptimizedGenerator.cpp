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

std::array<double, HOURS_PER_YEAR> OptimizedThermalGenerator::calculateAverageLoadTs(const Data::Area& area)
{
    std::array<double, HOURS_PER_YEAR> values;
    values.fill(2.5);
    return values;
}
std::array<double, HOURS_PER_YEAR> OptimizedThermalGenerator::calculateAverageRorTs(const Data::Area& area)
{
    std::array<double, HOURS_PER_YEAR> values;
    values.fill(2.5);
    return values;
}
std::array<double, HOURS_PER_YEAR> OptimizedThermalGenerator::calculateAverageRenewableTs(
  const Data::Area& area)
{
    std::array<double, HOURS_PER_YEAR> values;
    values.fill(2.5);
    return values;
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

        for (std::size_t i = 0; i < HOURS_PER_YEAR; ++i)
        {
            refValueLoad[i] += tmpLoad[i] * weights.load;
            refValueRor[i] += tmpRor[i] * weights.ror;
            refValueRenewable[i] += tmpRenewable[i] * weights.renewable;
        }
    }
    // calculate reference value
    for (std::size_t i = 0; i < HOURS_PER_YEAR; ++i)
        refValue[i] = refValueLoad[i] - refValueRor[i] - refValueRenewable[i];
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
