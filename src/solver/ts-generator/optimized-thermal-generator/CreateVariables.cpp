//
// Created by milos on 14/11/23.
//

#include "OptimizedGenerator.h"

namespace Antares::Solver::TSGenerator
{
void OptimizedThermalGenerator::buildProblemVariables()
{
    countVariables();
    buildEnsAndSpillageVariables();
    buildUnitPowerOutputVariables();
    buildStartEndMntVariables();
}

void OptimizedThermalGenerator::countVariables()
{
    return;
}

// create VARIABLES per day - ENS[t], Spill[t]
void OptimizedThermalGenerator::buildEnsAndSpillageVariables()
{
    return;
}

// create VARIABLES per day and per cluster-unit - P[t][u]
void OptimizedThermalGenerator::buildUnitPowerOutputVariables()
{
    return;
}

// create VARIABLES per day, per cluster-unit and per maintenance - s[t][u][m] & e[t][u][m]
void OptimizedThermalGenerator::buildStartEndMntVariables()
{
    return;
}

} // namespace Antares::Solver::TSGenerator
