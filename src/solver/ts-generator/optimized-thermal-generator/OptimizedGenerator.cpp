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

void OptimizedThermalGenerator::calculateResidualLoad(Data::MaintenanceGroup& maintenanceGroup)
{
    // dummy value for test
    std::array<double, 8760> values;
    values.fill(2.5);

    // extract some info about the group
    const std::string& groupName = maintenanceGroup.name();
    const auto& groupType = maintenanceGroup.type();
    const auto& groupResLoad = maintenanceGroup.getUsedResidualLoadTS();

    // for phase II
    if (groupType == Data::MaintenanceGroup::typeTimeserie)
    {
        // read user defined ts - userProvidedResidualLoadTS_ with getter - phase-II
        maintenanceGroup.setUsedResidualLoadTS(values);
    }

    // loop through the elements of weightMap weights_
    for (const auto& entryWeightMap : maintenanceGroup)
    {
        const auto& area = *(entryWeightMap.first);
        auto weights = (entryWeightMap.second);
        // we do not need to loop through clusters here
        // per area data -> load, ror, renewable is all we need to calculate
        // residual load TS
    }

    // set resLoadTS
    maintenanceGroup.setUsedResidualLoadTS(values);
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
