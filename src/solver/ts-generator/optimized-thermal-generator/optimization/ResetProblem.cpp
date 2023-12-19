//
// Created by milos on 14/11/23.
//

#include "../main/OptimizedGenerator.h"

namespace Antares::Solver::TSGenerator
{
void OptimizedThermalGenerator::resetProblem()
{
    // Clear the solver to reset it for the new problem
    solver.Clear();
    vars.ens.clear();
    vars.spill.clear();
    vars.clusterUnits.clear();
    return;
}

} // namespace Antares::Solver::TSGenerator
