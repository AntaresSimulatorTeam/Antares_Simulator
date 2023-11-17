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
    /*
    TEST optimization
    minimize:
    * 3x - y
    subject to:
    * 1.5 x + 2 y >= 12
    * 0 <= x <= 3
    * 0 <= y <= 5
    * x - linear variable
    * y - integer variable
    */

    // Define variables and at the same time Boundaries
    /*
    The bounds of a variable once set cannot be directly modified in OR-Tools.
    However, you can create a new variable with the desired bounds and
    constraints and use it in your optimization problem.
    */
    MPVariable* const x = solver.MakeNumVar(0.0, 3.0, "x");
    MPVariable* const y = solver.MakeIntVar(0.0, 5.0, "y");

    // Define constraints
    // TODO CR27: we need to make vector of shared pointers to MPVariable so we can access them in different files 
    MPConstraint* const ct = solver.MakeRowConstraint(12.0, infinity, "ct");
    ct->SetCoefficient(x, 1.5);
    ct->SetCoefficient(y, 2.0);

    // Define objective function
    MPObjective* const objective = solver.MutableObjective();
    objective->SetCoefficient(x, 3);
    objective->SetCoefficient(y, -1);
    objective->SetMinimization();

    // Count the number of variables in the solver
    int numVariables = solver.NumVariables();
    // Count the number of constraints in the solver
    int numConstraints = solver.NumConstraints();

    // Solve the problem
    const MPSolver::ResultStatus result_status = solver.Solve();

    if (result_status != MPSolver::OPTIMAL) {
        // If not optimal, print that optimization failed
        logs.info()<< "The problem does not have an optimal solution.\n";
        return;
    }
    // Access and print the results
    printf("Optimal objective value = %.2f\n", objective->Value());
    printf("x = %.2f\n", x->solution_value());
    printf("y = %.2f\n", y->solution_value());

    // Clear the solver to reset it for the new problem
    solver.Clear();

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
