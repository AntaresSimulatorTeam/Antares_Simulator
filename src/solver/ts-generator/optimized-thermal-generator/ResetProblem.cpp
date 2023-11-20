//
// Created by milos on 14/11/23.
//

#include "OptimizedGenerator.h"

namespace Antares::Solver::TSGenerator
{
void OptimizedThermalGenerator::resetProblem()
{
    // Clear the solver to reset it for the new problem
    solver.Clear();
    var.day.clear();
    return;
}

} // namespace Antares::Solver::TSGenerator
