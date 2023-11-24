//
// Created by milos on 14/11/23.
//

#include "OptimizedGenerator.h"

namespace Antares::Solver::TSGenerator
{
void OptimizedThermalGenerator::printProblemVarAndResults()
{
    printAllVariables();
    printConstraints();
    printObjectiveFunction(solver.MutableObjective());
}

void OptimizedThermalGenerator::printAllVariables()
{
    for (MPVariable* const variable : solver.variables())
    {
        std::cout << "Variable: " << variable->name() << ", "
                  << "Lower bound: " << variable->lb() << ", "
                  << "Upper bound: " << variable->ub() << std::endl;
    }

    std::cout << "total number of variables is: " << solver.NumVariables() << std::endl;

    return;
}

void OptimizedThermalGenerator::printConstraints()
{
    const int num_constraints = solver.NumConstraints();

    for (int i = 0; i < num_constraints; ++i)
    {
        const MPConstraint* const constraint = solver.constraint(i);
        std::cout << "**** Constraint " << i + 1 << " ****" << std::endl;
        std::cout << "Name: " << constraint->name() << std::endl;
        std::cout << "Lower Bound: " << constraint->lb() << std::endl;
        std::cout << "Upper Bound: " << constraint->ub() << std::endl;

        for (const auto& term : constraint->terms())
        {
            const MPVariable* const var = term.first;
            const double coefficient = term.second;
            std::cout << var->name() << ": " << coefficient << std::endl;
        }
        std::cout << "------------------------" << std::endl;
    }
}

void OptimizedThermalGenerator::printObjectiveFunction(MPObjective* objective)
{
    for (MPVariable* variable : solver.variables())
    {
        std::cout << variable->name() << ": " << objective->GetCoefficient(variable) << std::endl;
    }
    std::cout << std::endl;
    return;
}

} // namespace Antares::Solver::TSGenerator
