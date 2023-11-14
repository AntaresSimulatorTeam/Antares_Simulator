//
// Created by milos on 14/11/23.
//

#include "OptimizedGenerator.h"

namespace Antares::Solver::TSGenerator
{
// call all methods    
void OptimizedThermalGenerator::runOptimizationProblem()
{
    buildProblemVariables();
    setVariableBounds();
    buildProblemConstraints();
    setProblemCost();
    solveProblem();
    resetProblem();
}
// retrieve and check the results if optimization was successful
void OptimizedThermalGenerator::solveProblem()
{
    return;
}

} // namespace Antares::Solver::TSGenerator
