//
// Created by milos on 14/11/23.
//

#include "../main/OptimizedGenerator.h"

namespace Antares::Solver::TSGenerator
{

void OptimizationParameters::postScenarioOptimization(OptProblemSettings& optSett)
{
    // do not save if optimization failed at some step
    if (optSett.solved)
    {
        calculateScenarioResults();
        saveScenarioResults(optSett);
    }

    resetResultStorage();
    reSetDaysSinceLastMnt();
    reSetTimeHorizon();
    reSetNumberOfMaintenances();

    return;
}

void OptimizationParameters::calculateScenarioResults()
{
    // for each unit we have now scenarioResults_
    // which contains std::pairs of all [maintenanceStart, maintenanceDuration]
    // lets transfer that into vectors of UNIT availability

    // loop per units
    for (auto& unit : scenarioResults_)
    {
        unit.calculateAvailableDailyPower(scenarioLength_);
    }

    // now lets get CLUSTER availability by summing up UNIT availability

    // fill in with zeros
    for (auto& cluster : clusterData)
    {
        cluster.second.dynamicResults.availableDailyPower.resize(scenarioLength_ * DAYS_PER_YEAR);
    }

    // add one by one unit availability
    for (auto& unit : scenarioResults_)
    {
        auto& availableDailyPower
          = clusterData[unit.parentCluster].dynamicResults.availableDailyPower;

        std::transform(availableDailyPower.begin(),
                       availableDailyPower.end(),
                       unit.availableDailyPower.begin(),
                       availableDailyPower.begin(),
                       std::plus<double>());
    }

    // do not convert to hourly values yet
    // why waste memory - convert to hourly just before writing to ts
    // and then delete - local variable

    return;
}

void OptimizationParameters::saveScenarioResults(const OptProblemSettings& optSett)
{
    // loop through all clusters and write results
    // for one scenario into designated columns

    int colSaveFrom = optSett.scenario * scenarioLength_;
    int colSaveTo = colSaveFrom + scenarioLength_;

    // using on purpose this double loop
    // because looping through clusterData we cannot change cluster
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

void OptimizationParameters::saveScenarioResults(int fromCol,
                                                 int toCol,
                                                 Data::ThermalCluster& cluster)
{
    // daily results are in clusterData.availableDailyPower
    // convert to hourly values and store in cluster ts
    // we assume that vector availableDailyPower has:
    // scenarioLength_ * DAYS_PER_YEAR element
    // that we need to store inside columns from-to

    auto& availability = clusterData[&cluster].dynamicResults.availableDailyPower;
    assert((toCol - fromCol) * DAYS_PER_YEAR == availability.size());

    int vctCol = 0;
    for (int col = fromCol; col < toCol; ++col)
    {
        for (int row = 0; row < HOURS_PER_YEAR; ++row)
        {
            cluster.series.timeSeries[col][row] = availability[vctCol * 365 + (int)(row / 24)];
        }
        vctCol++;
    }
}

void OptimizationParameters::resetResultStorage()
{
    // clear units result structure
    scenarioResults_.clear();
    // clear cluster result structure
    // do not clear whole clusterData
    // we store input data here as well

    for (auto& cluster : clusterData)
    {
        cluster.second.dynamicResults.availableDailyPower.clear();
    }

    return;
}

void OptimizationParameters::reSetDaysSinceLastMnt()
{
    // we are back in first step, but not first scenario
    // we have messed up our values
    // we need to reset

    for (auto& cluster : clusterData)
    {
        cluster.second.dynamicInputs.daysSinceLastMaintenance
          = cluster.first->originalRandomlyGeneratedDaysSinceLastMaintenance;
    }

    return;
}

void OptimizationParameters::reSetTimeHorizon()
{
    // we are back in first step, but not first scenario
    // we have messed up our values
    // we need to reset

    timeHorizon_ = timeHorizonFirstStep_;
    return;
}

void OptimizationParameters::reSetNumberOfMaintenances()
{
    // we are back in first step, but not first scenario
    // we have messed up our values
    // we need to reset

    for (auto& cluster : clusterData)
    {
        cluster.second.dynamicInputs.numberOfMaintenances
          = cluster.second.staticInputs.numberOfMaintenancesFirstStep;
    }

    return;
}

} // namespace Antares::Solver::TSGenerator
