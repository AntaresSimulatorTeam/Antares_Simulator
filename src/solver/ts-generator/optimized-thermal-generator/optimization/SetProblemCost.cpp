//
// Created by milos on 14/11/23.
//

#include "../main/OptimizedGenerator.h"

namespace Antares::Solver::TSGenerator
{
// create OBJECTIVE FUNCTION - sum through [t] and sum through [u]
// sum[days]{ EnsCost*Ens[day] + SpillCost[day] + sum[units][ avgCost*P[t][u] ] }
void OptimizedThermalGenerator::setProblemCost(const OptProblemSettings& optSett)
{
    MPObjective* objective = solver.MutableObjective();

    setProblemEnsCost(objective);
    setProblemSpillCost(objective);
    setProblemPowerCost(optSett, objective);

    objective->SetMinimization();

    return;
}

void OptimizedThermalGenerator::setProblemEnsCost(MPObjective* objective)
{
    // loop per day
    for (const auto& ens : vars.ens)
    {
        objective->SetCoefficient(ens, par.ensCost_);
    }
    return;
}

void OptimizedThermalGenerator::setProblemSpillCost(MPObjective* objective)
{
    // loop per day
    for (const auto& spill : vars.spill)
    {
        objective->SetCoefficient(spill, par.spillCost_);
    }
    return;
}

void OptimizedThermalGenerator::setProblemPowerCost(const OptProblemSettings& optSett,
                                                    MPObjective* objective)
{
    // loop per units
    for (const auto& unit : vars.clusterUnits)
    {
        setProblemPowerCost(optSett, objective, unit);
    }
    return;
}

void OptimizedThermalGenerator::setProblemPowerCost(const OptProblemSettings& optSett,
                                                    MPObjective* objective,
                                                    const Unit& unit)
{
    // loop per day
    for (int day = 0; day < unit.P.size(); ++day)
    {
        double unitPowerCost = par.getPowerCost(*(unit.parentCluster), day + optSett.firstDay);
        objective->SetCoefficient(unit.P[day], unitPowerCost);
    }

    return;
}

} // namespace Antares::Solver::TSGenerator
