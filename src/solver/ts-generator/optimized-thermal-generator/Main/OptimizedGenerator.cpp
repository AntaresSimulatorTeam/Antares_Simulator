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
    allocateWhereToWriteTs();
    par.setMaintenanceGroupParameters();
    if (!par.checkMaintenanceGroupParameters())
        return;

    // loop through all scenarios
    for (std::size_t scenarioIndex = 0; scenarioIndex < scenarioNumber_; ++scenarioIndex)
    {
        OptProblemSettings optSett;
        optSett.firstDay = 0;
        optSett.lastDay = optSett.firstDay + par.timeHorizon_;
        optSett.scenario = scenarioIndex;

        // loop till the end of scenario length
        while (optSett.firstDay < scenarioLength_ * DAYS_PER_YEAR)
        {
            // check if the optimization was successful and exit loop otherwise
            if (!runOptimizationProblem(optSett))
                break;

            // Update the time values for the next iteration
            optSett.firstDay += par.timeStep_;
            optSett.lastDay = optSett.firstDay + par.timeHorizon_;
            optSett.isFirstStep = false;
        }
        par.postScenarioOptimization(optSett);
        ++progression_;
    }
    writeTsResults();
}

void OptimizedThermalGenerator::allocateWhereToWriteTs()
{
    // loop per areas inside maintenance group
    for (auto& entryWeightMap : maintenanceGroup_)
    {
        auto& area = *(entryWeightMap.first);
        // loop per thermal clusters inside the area
        for (auto& clusterEntry : area.thermal.list.mapping)
        {
            auto& cluster = *(clusterEntry.second);

            // check if cluster exist, do we generate + optimizeMaintenance
            // create start end variables only for these clusters
            bool genTS = checkClusterExist(cluster)
                         && cluster.doWeGenerateTS(globalThermalTSgeneration_)
                         && cluster.optimizeMaintenance;
            if (!genTS)
                continue;

            // allocate space
            cluster.series.timeSeries.reset(nbThermalTimeseries, 8760);
        }
    }
    return;
}

} // namespace Antares::Solver::TSGenerator
