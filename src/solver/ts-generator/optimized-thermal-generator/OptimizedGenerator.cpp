//
// Created by milos on 10/11/23.
//

#include "OptimizedGenerator.h"

#include <numeric>
#include <algorithm>

namespace Antares::Solver::TSGenerator
{
// optimization problem - methods
void OptimizedThermalGenerator::GenerateOptimizedThermalTimeSeries()
{
    setMaintenanceGroupParameters();
    if (!checkMaintenanceGroupParameters())
        return;

    // loop through all scenarios
    for (std::size_t scenarioIndex = 0; scenarioIndex < scenarioNumber_; ++scenarioIndex)
    {
        OptProblemSettings optSett;
        optSett.firstDay = 0;
        optSett.lastDay = timeHorizon_;
        // loop till the end of scenario length
        while (optSett.firstDay < scenarioLength_ * DAYS_PER_YEAR)
        {
            createOptimizationProblemPerGroup(optSett);

            // Update the time values for the next iteration
            optSett.firstDay += timeStep_;
            optSett.lastDay += timeStep_;
        }
    }
}

void OptimizedThermalGenerator::createOptimizationProblemPerGroup(const OptProblemSettings& optSett)
{
    runOptimizationProblem(optSett);
    // ...
    // do the optimization post-processing here &
    // do the writing off the result here

    // just playing here - will ue this loops later for opt problem formulation
    // loop through the elements of weightMap weights_
    for (const auto& entryWeightMap : maintenanceGroup_)
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
    // end play
}

void OptimizedThermalGenerator::createOptimizationProblemPerCluster(const Data::Area& area,
                                                                    Data::ThermalCluster& cluster)
{
    if (cluster.doWeGenerateTS(globalThermalTSgeneration_) && cluster.optimizeMaintenance)
    {
        // number of maintenances are defined per cluster-unit
        // but are the same for all units inside the same cluster
        int numberOfMaintenancesPerUnit;
        int averageMaintenanceDuration;
        numberOfMaintenancesPerUnit = calculateNumberOfMaintenances(
          cluster, timeHorizon_);
        averageMaintenanceDuration
          = calculateAverageMaintenanceDuration(cluster); // this will floor the double value !!
        std::array<double, DAYS_PER_YEAR> maxUnitOutput = calculateMaxUnitOutput(cluster);
        // uint earliestStartOfFirstMaintenance
        //   = calculateUnitEarliestStartOfFirstMaintenance(cluster, 8);
        // uint latestStartOfFirstMaintenance
        //   = calculateUnitLatestStartOfFirstMaintenance(cluster, 8);
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
