//
// Created by milos on 14/11/23.
//

#include "OptimizedGenerator.h"

namespace Antares::Solver::TSGenerator
{
void OptimizedThermalGenerator::buildProblemConstraints()
{
    buildLoadBalanceConstraints();
    setStartEndMntLogicConstraints();
    setMaxUnitOutputConstraints();
}

// load balance CONSTRAINTS - constraint-per-each-day[t] - we have sum through [u] inside of it
void OptimizedThermalGenerator::buildLoadBalanceConstraints()
{
    return;
}

// CONSTRAINTS per days, per units and per maintenance - constraint-per-each-day+unit+mnt[t][u][m]
void OptimizedThermalGenerator::setStartEndMntLogicConstraints()
{
    return;
}
// CONSTRAINTS per days and per units - constraint-per-each-day+unit[t][u][m-sum per m]
void OptimizedThermalGenerator::setMaxUnitOutputConstraints()
{
    return;
}

} // namespace Antares::Solver::TSGenerator
