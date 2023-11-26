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
