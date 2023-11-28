//
// Created by milos on 14/11/23.
//

#include "OptimizedGenerator.h"

namespace Antares::Solver::TSGenerator
{

void OptimizedThermalGenerator::postScenarioOptimization(OptProblemSettings& optSett)
{
    // do not save if optimization failed at some step
    if (!optSett.solved)
        return;

    calculateScenarioResults(optSett);
    saveScenarioResults(optSett);
    resetResultStorage();
    reSetDaysSinceLastMnt();

    return;
}

void OptimizedThermalGenerator::calculateScenarioResults(const OptProblemSettings& optSett)
{
    // for each unit we have now scenarioResults
    // which contains std::pairs of all [maintenanceStart, maintenanceDuration]
    // lets transfer that into vectors of UNIT availability

    // loop per units
    for (auto& unit : scenarioResults)
    {
        unit.calculateAvailableDailyPower(scenarioLength_);
    }

    // now lets get CLUSTER availability by summing up UNIT availability

    // fill in with zeros
    for (auto& cluster : maintenanceData)
    {
        cluster.second.availableClusterDailyPower.resize(scenarioLength_ * DAYS_PER_YEAR);
    }

    // add one by one unit availability
    for (auto& unit : scenarioResults)
    {
        auto& availableClusterDailyPower
          = maintenanceData[unit.parentCluster].availableClusterDailyPower;

        std::transform(availableClusterDailyPower.begin(),
                       availableClusterDailyPower.end(),
                       unit.availableDailyPower.begin(),
                       availableClusterDailyPower.begin(),
                       std::plus<double>());
    }

    // do not convert to hourly values yet
    // why waste memory - convert to hourly just before writing to ts
    // and then delete - local variable

    return;
}

void OptimizedThermalGenerator::saveScenarioResults(const OptProblemSettings& optSett)
{
    // loop through all clusters and write results
    // for one scenario into designated columns
    // daily results are in maintenanceData.availableClusterDailyPower
    // convert to hourly values and store in cluster ts

    int colSaveFrom = optSett.scenario * scenarioLength_;
    int colSaveTo = colSaveFrom + scenarioLength_ - 1;

    // using on purpose this double loop
    // because looping through maintenanceData we cannot change cluster
    // const Data::ThermalCluster*
    for (auto& entryWeightMap : maintenanceGroup_)
    {
        auto& area = *(entryWeightMap.first);
        for (auto& clusterEntry : area.thermal.list.mapping)
        {
            auto& cluster = *(clusterEntry.second);
            bool genTS = checkClusterExist(cluster)
                         && cluster.doWeGenerateTS(globalThermalTSgeneration_)
                         && cluster.optimizeMaintenance;
            if (!genTS)
                continue;

            // write results
            saveScenarioResults(colSaveFrom, colSaveTo, cluster);
        }
    }
    return;
}

void OptimizedThermalGenerator::saveScenarioResults(int from, int to, Data::ThermalCluster& cluster)
{
    cluster.series.timeSeries.fill(777.); // dummy for now
}

void OptimizedThermalGenerator::resetResultStorage()
{
    // clear units result structure
    scenarioResults.clear();
    // clear cluster result structure
    // do not clear whole maintenanceData
    // we store input data here as well

    for (auto& cluster : maintenanceData)
    {
        cluster.second.availableClusterDailyPower.clear();
    }

    return;
}

void OptimizedThermalGenerator::reSetDaysSinceLastMnt()
{
    // we are back in first step, but not first scenario
    // we have messed up our values
    // we need to re-do
    // daysSinceLastMaintenance = cluster.originalRandomlyGeneratedDaysSinceLastMaintenance;
    // for all areas and clusters

    for (auto& cluster : maintenanceData)
    {
        cluster.second.daysSinceLastMaintenance
          = cluster.first->originalRandomlyGeneratedDaysSinceLastMaintenance;
    }

    return;
}

// this method is called at the very end
// after all time-steps and scenarios
void OptimizedThermalGenerator::writeTsResults()
{
    // we need to loop through all the clusters
    // and write the results
    // it would be much easier to loop using maintenanceData
    // inside of it we already excluded all the non-important clusters
    // however it is const Data::ThermalCluster*
    // so we cannot modify cluster values

    for (auto& entryWeightMap : maintenanceGroup_)
    {
        auto& area = *(entryWeightMap.first);

        for (auto& clusterEntry : area.thermal.list.mapping)
        {
            auto& cluster = *(clusterEntry.second);

            if (!(checkClusterExist(cluster) && cluster.doWeGenerateTS(globalThermalTSgeneration_)
                  && cluster.optimizeMaintenance))
                continue;

            if (derated)
                cluster.series.timeSeries.averageTimeseries();

            if (archive)
                writeResultsToDisk(area, cluster);

            cluster.calculationOfSpinning();
        }
    }
    return;
}

} // namespace Antares::Solver::TSGenerator
