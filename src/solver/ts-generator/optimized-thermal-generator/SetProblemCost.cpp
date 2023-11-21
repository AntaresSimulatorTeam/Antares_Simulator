//
// Created by milos on 14/11/23.
//

#include "OptimizedGenerator.h"

namespace Antares::Solver::TSGenerator
{
// create OBJECTIVE FUNCTION - sum through [t] and sum through [u]
// sum[days]{ EnsCost*Ens[day] + SpillCost[day] + sum[units][ UnitCost*P[t][u] ] }
void OptimizedThermalGenerator::setProblemCost(const OptProblemSettings& optSett)
{
    MPObjective* objective = solver.MutableObjective();

    setProblemEnsCost(objective);
    setProblemSpillCost(objective);
    setProblemPowerCost(optSett, objective);

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

void OptimizedThermalGenerator::setProblemPowerCost(const OptProblemSettings& optSett,
                                                    MPObjective* objective)
{
    // loop per day
    for (int day = 0; day < timeHorizon_; ++day)
    {
        setProblemPowerCostPerGroup(optSett, objective, day);
    }
    return;
}

void OptimizedThermalGenerator::setProblemPowerCostPerGroup(const OptProblemSettings& optSett,
                                                            MPObjective* objective,
                                                            int day)
{
    // loop per area inside maintenance group
    for (const auto& entryWeightMap : maintenanceGroup_)
    {
        const auto& area = *(entryWeightMap.first);
        setProblemPowerCostPerArea(optSett, objective, day, area);
    }
    return;
}

void OptimizedThermalGenerator::setProblemPowerCostPerArea(const OptProblemSettings& optSett,
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

        setProblemPowerCostPerCluster(optSett, objective, day, area, cluster);
    }
    return;
}

void OptimizedThermalGenerator::setProblemPowerCostPerCluster(const OptProblemSettings& optSett,
                                                              MPObjective* objective,
                                                              int day,
                                                              const Data::Area& area,
                                                              const Data::ThermalCluster& cluster)
{
    /*
    ** Unit cost can be directly set,
    ** Or calculated using Fuel Cost, Co2 cost, Fuel Eff and V&O Cost.

    ** In second case we also need information which year it is (to choose proper TS number, and
    also what hour it is)
    ** we need price per day (so averaging the hourly values)
    ** this is NOT calculated prior to the simulation - so if we only want to run ts-gen, we cannot
    get this info just yet
    ** using:
    ** getMarginalCost(uint serieIndex, uint hourInTheYear) or
    ** getMarketBidCost(uint hourInTheYear, uint year)
    ** TODO CR27: maybe for phase-II
    ** for now just disable this option but take into account the thermalModulationCost!!
    */

    double unitPowerCost = returnUnitPowerCost(area, cluster, day + optSett.firstDay);
    // loop per unit inside the cluster
    for (int unit = 0; unit < cluster.unitCount; ++unit)
    {
        setProblemPowerCostPerUnit(objective, day, area, cluster, unit, unitPowerCost);
    }
    return;
}

void OptimizedThermalGenerator::setProblemPowerCostPerUnit(MPObjective* objective,
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
