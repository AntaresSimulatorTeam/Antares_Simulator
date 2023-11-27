//
// Created by milos on 14/11/23.
//

#include "OptimizedGenerator.h"

namespace Antares::Solver::TSGenerator
{

void OptimizedThermalGenerator::postTimeStepOptimization(OptProblemSettings& optSett)
{
    appendTimeStepResults(optSett);
    reCalculateDaysSinceLastMnt(optSett);
    return;
}

// save/append optimization results form range 0-timeStep
void OptimizedThermalGenerator::appendTimeStepResults(const OptProblemSettings& optSett)
{
    // we have vectors of start (zeros and ones)
    // lets convert that into maintenance start day vector
    // and then randomly generate maintenance duration
    // and create std::pairs - of start_day + mnt_duration

    // loop per units
    for (std::size_t unitIndexTotal = 0; unitIndexTotal < vars.clusterUnits.size();
         ++unitIndexTotal)
    {
        // Unit-unitIndexTotal - is index in a vector of all the units (area * cluster * units)
        // not to be confused by Unit-index - index in cluster

        // variables structure: vars and result structure: scenarioResults
        // are created at the same time in the same loop
        // so the order of the units should be the same !!!
        // so lets avoid creating some search/find method
        // that will find the Unit in scenarioResults according to its parentCluster and index
        // and just loop
        // assert parentCluster and index

        const auto& readResultUnit = vars.clusterUnits[unitIndexTotal];
        auto& storeResultUnit = scenarioResults[unitIndexTotal];

        assert(readResultUnit.parentCluster == storeResultUnit.parentCluster
               && "Read and Store Units do not point to the same parent cluster.");
        assert(readResultUnit.index == storeResultUnit.index
               && "Read and Store Units do not point to the same unit index.");

        // loop per maintenances of unit

        // TODO CR27: do we even have to loop through maintenances
        // or only see if first maintenance start before timeStep_
        // rest (second, third) maintenances will definitely happen after timeStep_ ?! 
        // Talk with Hugo

        // if createStartEndVariables for the readResultUnit is false
        // maintenances.size() is going to be zero - so in a way there is our check
        for (int mnt = 0; mnt < readResultUnit.maintenances.size(); ++mnt)
        {
            int localMaintenanceStart = readResultUnit.maintenances[mnt].startDay(timeStep_);
            if (localMaintenanceStart == -1)
                continue;
            int globalMaintenanceStart = localMaintenanceStart + optSett.firstDay;
            int maintenanceDuration = 50; // dummy
            storeResultUnit.maintenanceResults.push_back(
              std::make_pair(globalMaintenanceStart, maintenanceDuration));
        }
    }

    return;
}

// re-calculate parameters
void OptimizedThermalGenerator::reCalculateDaysSinceLastMnt(const OptProblemSettings& optSett)
{
    if (optSett.scenario == 0 && optSett.isFirstStep)
        return;
    if (optSett.scenario != 0 && optSett.isFirstStep)
    {
        // we are back in first step, but not first scenario we have messed up our values
        // we need to re-do
        // clusterVariables[&cluster].daysSinceLastMnt = cluster.daysSinceLastMaintenance;
        // for all areas and clusters
    }
    // TODO CR27: re-calculate days since last maintenance inputs if necessary
}

} // namespace Antares::Solver::TSGenerator
