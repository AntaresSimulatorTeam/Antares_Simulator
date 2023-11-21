//
// Created by milos on 14/11/23.
//

#include "OptimizedGenerator.h"

namespace Antares::Solver::TSGenerator
{
// create OBJECTIVE FUNCTION - sum through [t] and sum through [u]
// sum[days]{ EnsCost*Ens[day] + SpillCost[day] + sum[units][ UnitCost*P[t][u] ] }
void OptimizedThermalGenerator::setProblemCost()
{
    MPObjective* objective = solver.MutableObjective();

    setProblemEnsCost(objective);
    setProblemSpillCost(objective);
    setProblemPowerCost(objective);
    setProblemPowerCostPerGroup();
    setProblemPowerCostPerArea();
    setProblemPowerCostPerCluster();
    setProblemPowerCostPerUnit();

    objective->SetMinimization();

    printObjectiveFunction(objective);

    return;
}

void OptimizedThermalGenerator::setProblemEnsCost(MPObjective* objective)
{
    // loop per day
    for (int day = 0; day < timeHorizon_; ++day)
    {
        objective->SetCoefficient(var.day[day].Ens, ensCost_);
    }
    return;
}

void OptimizedThermalGenerator::setProblemSpillCost(MPObjective* objective)
{
    // loop per day
    for (int day = 0; day < timeHorizon_; ++day)
    {
        objective->SetCoefficient(var.day[day].Spill, spillCost_);
    }
    return;
}

void OptimizedThermalGenerator::setProblemPowerCost(MPObjective* objective)
{

}

void OptimizedThermalGenerator::setProblemPowerCostPerGroup()
{

}

void OptimizedThermalGenerator::setProblemPowerCostPerArea()
{

}

void OptimizedThermalGenerator::setProblemPowerCostPerCluster()
{

}

void OptimizedThermalGenerator::setProblemPowerCostPerUnit()
{

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
