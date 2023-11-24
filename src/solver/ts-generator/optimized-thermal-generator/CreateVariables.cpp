//
// Created by milos on 14/11/23.
//

#include "OptimizedGenerator.h"

namespace Antares::Solver::TSGenerator
{
void OptimizedThermalGenerator::buildProblemVariables(const OptProblemSettings& optSett)
{
    allocateVarStruct();
    buildEnsAndSpillageVariables(optSett);
    buildUnitPowerOutputVariables(optSett);
    buildStartEndMntVariables(optSett);
}

// populate OptimizationProblemVariables struct
void OptimizedThermalGenerator::allocateVarStruct()
{
    // loop per day
    for (int day = 0; day < timeHorizon_; ++day)
    {
        // fill the variable structure
        var.day.push_back(OptimizationProblemVariablesPerDay());
        allocateVarStruct(day);
    }
}

void OptimizedThermalGenerator::allocateVarStruct(int day)
{
    // loop per areas inside maintenance group
    for (const auto& entryWeightMap : maintenanceGroup_)
    {
        const auto& area = *(entryWeightMap.first);
        auto& areaVariables = var.day[day].areaMap;
        areaVariables[&area] = OptimizationProblemVariablesPerArea();
        allocateVarStruct(day, area);
    }
}

void OptimizedThermalGenerator::allocateVarStruct(int day, const Data::Area& area)
{
    // loop per thermal clusters inside the area
    for (const auto& clusterEntry : area.thermal.list.mapping)
    {
        const auto& cluster = *(clusterEntry.second);

        // we do not check if cluster.optimizeMaintenance = true here
        // we add all the clusters Power inside maintenance group
        if (!checkClusterExist(cluster))
            continue;

        auto& clusterVariables = var.day[day].areaMap[&area].clusterMap;
        clusterVariables[&cluster] = OptimizationProblemVariablesPerCluster();
        allocateVarStruct(day, cluster);
    }
}

void OptimizedThermalGenerator::allocateVarStruct(int day, const Data::ThermalCluster& cluster)
{
    // loop per unit inside the cluster
    for (int unit = 0; unit < cluster.unitCount; ++unit)
    {
        var.day[day].areaMap[cluster.parentArea].clusterMap[&cluster].unitMap.push_back(
          OptimizationProblemVariablesPerUnit());
    }
}

// create VARIABLES per day - ENS[t], Spill[t]
void OptimizedThermalGenerator::buildEnsAndSpillageVariables(const OptProblemSettings& optSett)
{
    for (int day = 0; day < timeHorizon_; ++day)
    {
        // add ENS variables
        var.day[day].Ens = solver.MakeNumVar(
          0.0, solverInfinity, "ENS_[" + std::to_string(day + optSett.firstDay) + "]");
    }

    for (int day = 0; day < timeHorizon_; ++day)
    {
        // add Spillage variables
        var.day[day].Spill = solver.MakeNumVar(
          0.0, solverInfinity, "Spill_[" + std::to_string(day + optSett.firstDay) + "]");
    }

    return;
}

// create VARIABLES per day and per cluster-unit - P[t][u]
void OptimizedThermalGenerator::buildUnitPowerOutputVariables(const OptProblemSettings& optSett)
{
    // loop per areas inside maintenance group
    for (const auto& entryWeightMap : maintenanceGroup_)
    {
        const auto& area = *(entryWeightMap.first);
        buildUnitPowerOutputVariables(optSett, area);
    }
}

void OptimizedThermalGenerator::buildUnitPowerOutputVariables(const OptProblemSettings& optSett,
                                                              const Data::Area& area)
{
    // loop per thermal clusters inside the area
    for (const auto& clusterEntry : area.thermal.list.mapping)
    {
        const auto& cluster = *(clusterEntry.second);

        // we do not check if cluster.optimizeMaintenance = true here
        // we add all the clusters Power inside maintenance group
        if (!checkClusterExist(cluster))
            continue;

        buildUnitPowerOutputVariables(optSett, cluster);
    }
}

void OptimizedThermalGenerator::buildUnitPowerOutputVariables(const OptProblemSettings& optSett,
                                                              const Data::ThermalCluster& cluster)
{
    // loop per units inside the cluster
    for (int unit = 0; unit < cluster.unitCount; ++unit)
    {
        buildUnitPowerOutputVariables(optSett, cluster, unit);
    }
}

void OptimizedThermalGenerator::buildUnitPowerOutputVariables(const OptProblemSettings& optSett,
                                                              const Data::ThermalCluster& cluster,
                                                              int unit)
{
    // loop per day
    for (int day = 0; day < timeHorizon_; ++day)
    {
        // add P[t][u] variables
        var.day[day].areaMap[cluster.parentArea].clusterMap[&cluster].unitMap[unit].P
          = solver.MakeNumVar(0.0,
                              solverInfinity,
                              "P_[" + std::to_string(day + optSett.firstDay) + "]["
                                + cluster.getFullName().to<std::string>() + "."
                                + std::to_string(unit) + "]");
    }

    return;
}

// create VARIABLES per day, per cluster-unit and per maintenance - s[t][u][m] & e[t][u][m]
void OptimizedThermalGenerator::buildStartEndMntVariables(const OptProblemSettings& optSett)
{
    // loop per areas inside maintenance group
    for (const auto& entryWeightMap : maintenanceGroup_)
    {
        const auto& area = *(entryWeightMap.first);
        buildStartEndMntVariables(optSett, area);
    }
}

void OptimizedThermalGenerator::buildStartEndMntVariables(const OptProblemSettings& optSett,
                                                          const Data ::Area& area)
{
    // loop per thermal clusters inside the area
    for (const auto& clusterEntry : area.thermal.list.mapping)
    {
        const auto& cluster = *(clusterEntry.second);

        // check if cluster exist, do we generate + optimizeMaintenance
        // create start end variables only for these clusters
        bool createStartEndVar = checkClusterExist(cluster)
                                 && cluster.doWeGenerateTS(globalThermalTSgeneration_)
                                 && cluster.optimizeMaintenance;
        if (!createStartEndVar)
            continue;

        buildStartEndMntVariables(optSett, cluster);
    }
}

void OptimizedThermalGenerator::buildStartEndMntVariables(const OptProblemSettings& optSett,
                                                          const Data ::ThermalCluster& cluster)
{
    int totalMntNumber = getNumberOfMaintenances(cluster);

    // loop per units inside the cluster
    for (int unit = 0; unit < cluster.unitCount; ++unit)
    {
        buildStartEndMntVariables(optSett, cluster, unit, totalMntNumber);
    }
}

void OptimizedThermalGenerator::buildStartEndMntVariables(const OptProblemSettings& optSett,
                                                          const Data ::ThermalCluster& cluster,
                                                          int unit,
                                                          int totalMntNumber)
{
    // loop per maintenances per unit
    for (int mnt = 0; mnt < totalMntNumber; ++mnt)
    {
        buildStartVariables(optSett, cluster, unit, mnt);
        buildEndVariables(optSett, cluster, unit, mnt);
    }
}

void OptimizedThermalGenerator::buildStartVariables(const OptProblemSettings& optSett,
                                                    const Data ::ThermalCluster& cluster,
                                                    int unit,
                                                    int mnt)
{
    // loop per day
    for (int day = 0; day < timeHorizon_; ++day)
    {
        // add start[t][u][m] variables
        var.day[day].areaMap[cluster.parentArea].clusterMap[&cluster].unitMap[unit].start.push_back(
          solver.MakeIntVar(0.0,
                            1.0,
                            "S_[" + std::to_string(day + optSett.firstDay) + "]["
                              + cluster.getFullName().to<std::string>() + "." + std::to_string(unit)
                              + "][" + std::to_string(mnt) + "]"));
    }
    return;
}

void OptimizedThermalGenerator::buildEndVariables(const OptProblemSettings& optSett,
                                                  const Data ::ThermalCluster& cluster,
                                                  int unit,
                                                  int mnt)
{
    // loop per day
    for (int day = 0; day < timeHorizon_; ++day)
    {
        // add end[t][u][m] variables
        var.day[day].areaMap[cluster.parentArea].clusterMap[&cluster].unitMap[unit].end.push_back(
          solver.MakeIntVar(0.0,
                            1.0,
                            "E_[" + std::to_string(day + optSett.firstDay) + "]["
                              + cluster.getFullName().to<std::string>() + "." + std::to_string(unit)
                              + "][" + std::to_string(mnt) + "]"));
    }
    return;
}

} // namespace Antares::Solver::TSGenerator
