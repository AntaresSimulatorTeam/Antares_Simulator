//
// Created by milos on 14/11/23.
//

#include "OptimizedGenerator.h"

namespace Antares::Solver::TSGenerator
{
// call all methods
bool OptimizedThermalGenerator::runOptimizationProblem(OptProblemSettings& optSett)
{
    resetProblem();
    reCalculateDaysSinceLastMnt(optSett);
    buildProblemVariables(optSett);
    fixBounds();
    buildProblemConstraints(optSett);
    setProblemCost(optSett);
    if (!solveProblem(optSett))
        return false;
    printProblemVarAndResults();
    appendStepResults();
    return true;
}

// retrieve and check the results if optimization was successful
bool OptimizedThermalGenerator::solveProblem(OptProblemSettings& optSett)
{
    // Solve the problem
    const MPSolver::ResultStatus result_status = solver.Solve();

    if (result_status != MPSolver::OPTIMAL)
    {
        // If not optimal, print that optimization failed
        optSett.solved = false;
        logs.warning() << "Maintenance group: " << maintenanceGroup_.name()
                       << ". Scenario Num: " << optSett.scenario
                       << ". Optimization failed in step: " << optSett.firstDay << ".Day - "
                       << optSett.lastDay << ".Day. This scenario wont have generated timeseries";
        return false;
    }
    return true;
}

// TODO CR27: move this method somewhere else
// collect and store results form firstDay-lastDay
void OptimizedThermalGenerator::appendStepResults()
{
    return;
}

} // namespace Antares::Solver::TSGenerator
