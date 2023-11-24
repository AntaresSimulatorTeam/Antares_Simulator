//
// Created by milos on 14/11/23.
//

#include "OptimizedGenerator.h"

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

void OptimizedThermalGenerator::setProblemPowerCost(const OptProblemSettings& optSett,
                                                    MPObjective* objective)
{
    // loop per day
    for (int day = 0; day < timeHorizon_; ++day)
    {
        setProblemPowerCost(optSett, objective, day);
    }
    return;
}

void OptimizedThermalGenerator::setProblemPowerCost(const OptProblemSettings& optSett,
                                                    MPObjective* objective,
                                                    int day)
{
    // loop per area inside maintenance group
    for (const auto& entryWeightMap : maintenanceGroup_)
    {
        const auto& area = *(entryWeightMap.first);
        setProblemPowerCost(optSett, objective, day, area);
    }
    return;
}

void OptimizedThermalGenerator::setProblemPowerCost(const OptProblemSettings& optSett,
                                                    MPObjective* objective,
                                                    int day,
                                                    const Data::Area& area)
{
    // loop per thermal clusters inside the area
    for (auto it = area.thermal.list.mapping.begin(); it != area.thermal.list.mapping.end(); ++it)
    {
        const auto& cluster = *(it->second);

        // we do not check if cluster.optimizeMaintenance = true here
        // we add all the clusters Power inside maintenance group
        if (!checkClusterExist(cluster))
            continue;

        setProblemPowerCost(optSett, objective, day, area, cluster);
    }
    return;
}

void OptimizedThermalGenerator::setProblemPowerCost(const OptProblemSettings& optSett,
                                                    MPObjective* objective,
                                                    int day,
                                                    const Data::Area& area,
                                                    const Data::ThermalCluster& cluster)
{
    double unitPowerCost = getPowerCost(cluster, day + optSett.firstDay);
    // loop per unit inside the cluster
    for (int unit = 0; unit < cluster.unitCount; ++unit)
    {
        setProblemPowerCost(objective, day, area, cluster, unit, unitPowerCost);
    }
    return;
}

void OptimizedThermalGenerator::setProblemPowerCost(MPObjective* objective,
                                                    int day,
                                                    const Data::Area& area,
                                                    const Data::ThermalCluster& cluster,
                                                    int unitIndex,
                                                    double cost)
{
    objective->SetCoefficient(var.day[day].areaMap[&area].clusterMap[&cluster].unitMap[unitIndex].P,
                              cost);
    return;
}

} // namespace Antares::Solver::TSGenerator
