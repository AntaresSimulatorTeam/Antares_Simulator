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

    return;
}

void OptimizedThermalGenerator::calculateScenarioResults(const OptProblemSettings& optSett)
{
    return;
}

void OptimizedThermalGenerator::saveScenarioResults(const OptProblemSettings& optSett)
{
    // save results

    int colSaveFrom = optSett.scenario * scenarioLength_;
    int colSaveTo = colSaveFrom + scenarioLength_ - 1;

    // loop through all areas and clusters and write results for one scenario
}

void OptimizedThermalGenerator::resetResultStorage()
{
    scenarioResults.clear();
    return;
}

// this method is called at the very end
// after all time-steps and scenarios
void OptimizedThermalGenerator::writeTsResults()
{
    // we need to loop through all the clusters
    // and write the results
    // it would be much easier to loop using MaintenanceData structure
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
