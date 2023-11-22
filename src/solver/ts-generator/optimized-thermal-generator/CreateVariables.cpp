//
// Created by milos on 14/11/23.
//

#include "OptimizedGenerator.h"

namespace Antares::Solver::TSGenerator
{
void OptimizedThermalGenerator::buildProblemVariables(const OptProblemSettings& optSett)
{
    // this is now just dummy optimization problem
    countVariables();

    allocateVarStruct();
    buildEnsAndSpillageVariables(optSett);
    buildUnitPowerOutputVariables(optSett);
    buildStartEndMntVariables(optSett);
    printAllVariables();
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
    MPVariable* x = solver.MakeNumVar(0.0, 3.0, "x");
    MPVariable* y = solver.MakeIntVar(0.0, 5.0, "y");

    // redefine lower bound - just do not make x - constant
    x->SetLB(1.5);

    // Define constraints
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

    if (result_status != MPSolver::OPTIMAL)
    {
        // If not optimal, print that optimization failed
        logs.info() << "The problem does not have an optimal solution.\n";
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

// populate OptimizationProblemVariables struct
void OptimizedThermalGenerator::allocateVarStruct()
{
    // loop per day
    for (int day = 0; day < timeHorizon_; ++day)
    {
        // fill the variable structure
        var.day.push_back(OptimizationProblemVariablesPerDay());
        allocateVarStructPerGroup(day);
    }
}

void OptimizedThermalGenerator::allocateVarStructPerGroup(int day)
{
    // loop per areas inside maintenance group
    for (const auto& entryWeightMap : maintenanceGroup_)
    {
        const auto& area = *(entryWeightMap.first);
        auto& areaVariables = var.day[day].areaMap;
        areaVariables[&area] = OptimizationProblemVariablesPerArea();
        allocateVarStructPerArea(day, area);
    }
}

void OptimizedThermalGenerator::allocateVarStructPerArea(int day, const Data::Area& area)
{
    // loop per thermal clusters inside the area
    for (auto it = area.thermal.list.mapping.begin(); it != area.thermal.list.mapping.end(); ++it)
    {
        const auto& cluster = *(it->second);

        // we do not check if cluster.optimizeMaintenance = true here
        // we add all the clusters Power inside maintenance group
        if (!checkClusterExist(cluster))
            continue;

        auto& clusterVariables = var.day[day].areaMap[&area].clusterMap;
        clusterVariables[&cluster] = OptimizationProblemVariablesPerCluster();
        allocateVarStructPerCluster(day, cluster);
    }
}

void OptimizedThermalGenerator::allocateVarStructPerCluster(int day,
                                                            const Data::ThermalCluster& cluster)
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
          0.0, infinity, "ENS_[" + std::to_string(day + optSett.firstDay) + "]");
    }

    for (int day = 0; day < timeHorizon_; ++day)
    {
        // add Spillage variables
        var.day[day].Spill = solver.MakeNumVar(
          0.0, infinity, "Spill_[" + std::to_string(day + optSett.firstDay) + "]");
    }

    return;
}

// create VARIABLES per day and per cluster-unit - P[t][u]
void OptimizedThermalGenerator::buildUnitPowerOutputVariables(const OptProblemSettings& optSett)
{
    // loop per day
    for (int day = 0; day < timeHorizon_; ++day)
    {
        buildUnitPowerOutputVariablesPerGroup(optSett, day);
    }
    return;
}

void OptimizedThermalGenerator::buildUnitPowerOutputVariablesPerGroup(
  const OptProblemSettings& optSett,
  int day)
{
    // loop per areas inside maintenance group
    for (const auto& entryWeightMap : maintenanceGroup_)
    {
        const auto& area = *(entryWeightMap.first);
        buildUnitPowerOutputVariablesPerArea(optSett, day, area);
    }
}

void OptimizedThermalGenerator::buildUnitPowerOutputVariablesPerArea(
  const OptProblemSettings& optSett,
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

        buildUnitPowerOutputVariablesPerCluster(optSett, day, cluster);
    }
}

void OptimizedThermalGenerator::buildUnitPowerOutputVariablesPerCluster(
  const OptProblemSettings& optSett,
  int day,
  const Data::ThermalCluster& cluster)
{
    // loop per units inside the cluster
    for (int unit = 0; unit < cluster.unitCount; ++unit)
    {
        // add P[t][u] variables
        var.day[day].areaMap[cluster.parentArea].clusterMap[&cluster].unitMap[unit].P
          = solver.MakeNumVar(0.0,
                              infinity,
                              "P_[" + std::to_string(day + optSett.firstDay) + "]["
                                + cluster.getFullName().to<std::string>() + "."
                                + std::to_string(unit) + "]");
    }
}

// create VARIABLES per day, per cluster-unit and per maintenance - s[t][u][m] & e[t][u][m]
void OptimizedThermalGenerator::buildStartEndMntVariables(const OptProblemSettings& optSett)
{
    // loop per day
    for (int day = 0; day < timeHorizon_; ++day)
    {
        buildStartEndMntVariablesPerGroup(optSett, day);
    }
    return;
}

void OptimizedThermalGenerator::buildStartEndMntVariablesPerGroup(const OptProblemSettings& optSett,
                                                                  int day)
{
    // loop per areas inside maintenance group
    for (const auto& entryWeightMap : maintenanceGroup_)
    {
        const auto& area = *(entryWeightMap.first);
        buildStartEndMntVariablesPerArea(optSett, day, area);
    }
}

void OptimizedThermalGenerator::buildStartEndMntVariablesPerArea(const OptProblemSettings& optSett,
                                                                 int day,
                                                                 const Data ::Area& area)
{
    // loop per thermal clusters inside the area
    for (auto it = area.thermal.list.mapping.begin(); it != area.thermal.list.mapping.end(); ++it)
    {
        const auto& cluster = *(it->second);

        // check if cluster exist, do we generate + optimizeMaintenance
        // create start end variables only for these clusters
        bool createStartEndVar = checkClusterExist(cluster)
                                 && cluster.doWeGenerateTS(globalThermalTSgeneration_)
                                 && cluster.optimizeMaintenance;
        if (!createStartEndVar)
            continue;

        buildStartEndMntVariablesPerCluster(optSett, day, cluster);
    }
}

void OptimizedThermalGenerator::buildStartEndMntVariablesPerCluster(
  const OptProblemSettings& optSett,
  int day,
  const Data ::ThermalCluster& cluster)
{
    int totalMntNumber = calculateNumberOfMaintenances(cluster, timeHorizon_);
    // loop per units inside the cluster
    for (int unit = 0; unit < cluster.unitCount; ++unit)
    {
        buildStartEndMntVariablesPerUnit(optSett, day, cluster, unit, totalMntNumber);
    }
}

void OptimizedThermalGenerator::buildStartEndMntVariablesPerUnit(
  const OptProblemSettings& optSett,
  int day,
  const Data ::ThermalCluster& cluster,
  int unit,
  int totalMntNumber)
{
    // loop per maintenances per unit
    for (int mnt = 0; mnt < totalMntNumber; ++mnt)
    {
        // add start[t][u][m] variables
        var.day[day].areaMap[cluster.parentArea].clusterMap[&cluster].unitMap[unit].start.push_back(
          solver.MakeIntVar(0.0,
                            1.0,
                            "S_[" + std::to_string(day + optSett.firstDay) + "]["
                              + cluster.getFullName().to<std::string>() + "." + std::to_string(unit)
                              + "][" + std::to_string(mnt) + "]"));

        // add end[t][u][m] variables
        var.day[day].areaMap[cluster.parentArea].clusterMap[&cluster].unitMap[unit].end.push_back(
          solver.MakeIntVar(0.0,
                            1.0,
                            "E_[" + std::to_string(day + optSett.firstDay) + "]["
                              + cluster.getFullName().to<std::string>() + "." + std::to_string(unit)
                              + "][" + std::to_string(mnt) + "]"));
    }
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

} // namespace Antares::Solver::TSGenerator
