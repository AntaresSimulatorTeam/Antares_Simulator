//
// Created by milos on 14/11/23.
//

#include "../main/OptimizedGenerator.h"

namespace Antares::Solver::TSGenerator
{
void OptimizedThermalGenerator::buildProblemVariables(const OptProblemSettings& optSett)
{
    buildEnsAndSpillageVariables(optSett);
    buildUnitPowerOutputVariables(optSett);
}

// create VARIABLES per day - ENS[t], Spill[t]
void OptimizedThermalGenerator::buildEnsAndSpillageVariables(const OptProblemSettings& optSett)
{
    for (int day = 0; day < par.timeHorizon_; ++day)
    {
        // add ENS[t] variables
        vars.ens.push_back(solver.MakeNumVar(
          0.0, solverInfinity, "ENS_[" + std::to_string(day + optSett.firstDay) + "]"));
    }

    for (int day = 0; day < par.timeHorizon_; ++day)
    {
        // add Spillage[t] variables
        vars.spill.push_back(solver.MakeNumVar(
          0.0, solverInfinity, "Spill_[" + std::to_string(day + optSett.firstDay) + "]"));
    }
    return;
}

// create VARIABLES per day and per cluster-unit - P[u][t]

void OptimizedThermalGenerator::buildUnitPowerOutputVariables(const OptProblemSettings& optSett)
{
    // loop per thermal clusters inside the area
    for (const auto& clusterEntry : par.clusterData)
    {
        const auto& cluster = *(clusterEntry.first);
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
    // add new Unit
    vars.clusterUnits.push_back(Unit());

    // but we do not know the total unit count
    // so always retrieve the last one
    auto& unitRef = vars.clusterUnits.back();

    // fill in data for the Unit
    unitRef.parentCluster = &cluster;
    unitRef.index = unit; // local count inside the cluster
    unitRef.createStartEndVariables = true;

    // if we are in the first step
    // lets add Unit, with inputs, to the scenarioResults
    if (optSett.isFirstStep)
        scenarioResults.push_back(vars.clusterUnits.back());

    // loop per day
    for (int day = 0; day < par.timeHorizon_; ++day)
    {
        // add P[u][t] variables
        unitRef.P.push_back(solver.MakeNumVar(0.0,
                                              solverInfinity,
                                              "P_[" + cluster.getFullName().to<std::string>() + "."
                                                + std::to_string(unit) + "]["
                                                + std::to_string(day + optSett.firstDay) + "]"));
    }

    // check if: do we generate + optimizeMaintenance
    // create start end variables only for these clusters
    if (!(cluster.doWeGenerateTS(globalThermalTSgeneration_) && cluster.optimizeMaintenance))
    {
        unitRef.createStartEndVariables = false;
        if (optSett.isFirstStep)
            scenarioResults.back().createStartEndVariables = false;
        return;
    }

    buildStartEndMntVariables(optSett, cluster, unit, unitRef);

    return;
}

void OptimizedThermalGenerator::buildStartEndMntVariables(const OptProblemSettings& optSett,
                                                          const Data ::ThermalCluster& cluster,
                                                          int unit,
                                                          Unit& unitRef)
{
    int totalMntNumber = par.getNumberOfMaintenances(cluster, unit);
    // loop per maintenances per unit
    for (int mnt = 0; mnt < totalMntNumber; ++mnt)
    {
        unitRef.maintenances.push_back(Maintenances());
        buildStartVariables(optSett, cluster, unit, unitRef, mnt);
        buildEndVariables(optSett, cluster, unit, unitRef, mnt);
    }
}

void OptimizedThermalGenerator::buildStartVariables(const OptProblemSettings& optSett,
                                                    const Data ::ThermalCluster& cluster,
                                                    int unit,
                                                    Unit& unitRef,
                                                    int mnt)
{
    // loop per day
    for (int day = 0; day < par.timeHorizon_; ++day)
    {
        // add start[u][m][t] variables
        unitRef.maintenances.back().start.push_back(solver.MakeIntVar(
          0.0,
          1.0,
          "S_[" + cluster.getFullName().to<std::string>() + "." + std::to_string(unit) + "]["
            + std::to_string(mnt) + "][" + std::to_string(day + optSett.firstDay) + "]"));
    }
    return;
}

void OptimizedThermalGenerator::buildEndVariables(const OptProblemSettings& optSett,
                                                  const Data ::ThermalCluster& cluster,
                                                  int unit,
                                                  Unit& unitRef,
                                                  int mnt)
{
    // loop per day
    for (int day = 0; day < par.timeHorizon_; ++day)
    {
        // add end[u][m][t] variables
        unitRef.maintenances.back().end.push_back(solver.MakeIntVar(
          0.0,
          1.0,
          "E_[" + cluster.getFullName().to<std::string>() + "." + std::to_string(unit) + "]["
            + std::to_string(mnt) + "][" + std::to_string(day + optSett.firstDay) + "]"));
    }
    return;
}

} // namespace Antares::Solver::TSGenerator
