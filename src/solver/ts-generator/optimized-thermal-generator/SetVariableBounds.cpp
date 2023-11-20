//
// Created by milos on 14/11/23.
//

#include "OptimizedGenerator.h"

namespace Antares::Solver::TSGenerator
{
void OptimizedThermalGenerator::setVariableBounds()
{
    setEnsAndSpillageBounds();
    setUnitPowerOutputBounds();
    setStartEndMntBounds();
    setFirstMntStartBounds();
    setAllMntMustStartBounds();
}

// BOUNDS per each day - bounds-per-each-day[t] - ENS[t] >= 0, Spill[t] >= 0
void OptimizedThermalGenerator::setEnsAndSpillageBounds()
{
    // no need for this - defined when creating the Ens & Spill variables
    return;
}

// BOUNDS per day and per each unit - bounds-per-each-day+unit[t][u] - P[t][u] >= 0
void OptimizedThermalGenerator::setUnitPowerOutputBounds()
{
    // no need for this - defined when creating the P variables
    return;
}

// BOUNDS per day, per each unit and per each mnt - bounds-per-each-day+unit+mnt[t][u][m]
// - s[t][u][m]-> [0, 1] and e[t][u][m]-> [0, 1]
void OptimizedThermalGenerator::setStartEndMntBounds()
{
    // no need for this - defined when creating the Start & End variables
    return;
}

// TODO CR27: see if to make this bound or constraint -
// it is definitely easier to do set it as a fix bound -
// but the solver might go crazy - as for adq.patch

// BOUNDS/CONSTRAINTS per units - constraint-per-each-unit[t-fixed][u][m-fixed]
// first maintenance must start between tauLower and tauUpper
// start[tauLower-1][u][1] = 0
// start[tauUpper][u][1] = 1
void OptimizedThermalGenerator::setFirstMntStartBounds()
{
    return;
}

// BOUNDS/CONSTRAINTS per units and per maintenance - constraint-per-each-unit+mnt[t-fixed=0/T][u][m]
// end[0][u][q] = 0 // no maintenance can end in first day
// start[T][u][q] = 1 // T - end Day (simulation end) // all maintenance must start till last day
void OptimizedThermalGenerator::setAllMntMustStartBounds()
{
    return;
}

} // namespace Antares::Solver::TSGenerator
