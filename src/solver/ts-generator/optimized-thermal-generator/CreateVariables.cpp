//
// Created by milos on 14/11/23.
//

#include "OptimizedGenerator.h"

namespace Antares::Solver::TSGenerator
{
void OptimizedThermalGenerator::buildProblemVariables(const OptProblemSettings& optSett)
{
    countVariables();
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

// create VARIABLES per day - ENS[t], Spill[t]
void OptimizedThermalGenerator::buildEnsAndSpillageVariables(const OptProblemSettings& optSett)
{
    for (int day = 0; day < timeHorizon_; ++day)
    {
        // fill the variable structure
        var.day.push_back(OptimizationProblemVariablesPerDay());
    }

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
    // loop per day - structure already filled in
    for (int day = 0; day < timeHorizon_; ++day)
    {
        // loop per area inside maintenance group - fill in the structure
        for (const auto& entryWeightMap : maintenanceGroup_)
        {
            const auto& area = *(entryWeightMap.first);
            auto& areaVariables = var.day[day].areaMap;
            areaVariables[&area] = OptimizationProblemVariablesPerArea();

            // loop per thermal clusters inside the area - fill in the structure
            for (auto it = area.thermal.list.mapping.begin(); it != area.thermal.list.mapping.end();
                 ++it)
            {
                const auto& cluster = *(it->second);

                // we do not check if cluster.optimizeMaintenance = true here
                // we add all the clusters Power inside maintenance group
                if (!checkClusterExist(cluster))
                    continue;

                auto& clusterVariables = areaVariables[&area].clusterMap;
                clusterVariables[&cluster] = OptimizationProblemVariablesPerCluster();

                // loop per unit inside the cluster - fill in the structure
                for (int unit = 0; unit < cluster.unitCount; ++unit)
                {
                    clusterVariables[&cluster].unitMap.push_back(
                      OptimizationProblemVariablesPerUnit());
                }
            }
        }
    }

    // loop per day
    for (int day = 0; day < timeHorizon_; ++day)
    {
        // loop per area inside maintenance group
        for (const auto& entryWeightMap : maintenanceGroup_)
        {
            const auto& area = *(entryWeightMap.first);
            auto& areaVariables = var.day[day].areaMap;

            // loop per thermal clusters inside the area
            for (auto it = area.thermal.list.mapping.begin(); it != area.thermal.list.mapping.end();
                 ++it)
            {
                const auto& cluster = *(it->second);

                // we do not check if cluster.optimizeMaintenance = true here
                // we add all the clusters Power inside maintenance group
                if (!checkClusterExist(cluster))
                    continue;

                auto& clusterVariables = areaVariables[&area].clusterMap;

                // loop per unit inside the cluster
                for (int unit = 0; unit < cluster.unitCount; ++unit)
                {
                    // add P[t][u] variables
                    clusterVariables[&cluster].unitMap[unit].P
                      = solver.MakeNumVar(0.0,
                                          infinity,
                                          "P_[" + std::to_string(day + optSett.firstDay) + "]["
                                            + cluster.getFullName().to<std::string>() + "."
                                            + std::to_string(unit) + "]");
                }
            }
        }
    }

    return;
}

// create VARIABLES per day, per cluster-unit and per maintenance - s[t][u][m] & e[t][u][m]
void OptimizedThermalGenerator::buildStartEndMntVariables(const OptProblemSettings& optSett)
{
    // loop per day
    for (int day = 0; day < timeHorizon_; ++day)
    {
        // loop per area inside maintenance group
        for (const auto& entryWeightMap : maintenanceGroup_)
        {
            const auto& area = *(entryWeightMap.first);
            auto& areaVariables = var.day[day].areaMap;

            // loop per thermal clusters inside the area
            for (auto it = area.thermal.list.mapping.begin(); it != area.thermal.list.mapping.end();
                 ++it)
            {
                const auto& cluster = *(it->second);

                // check if cluster exist, do we generate + optimizeMaintenance
                // create start end variables only for these clusters
                bool createStartEndVar = checkClusterExist(cluster)
                                         && cluster.doWeGenerateTS(globalThermalTSgeneration_)
                                         && cluster.optimizeMaintenance;
                if (!createStartEndVar)
                    continue;

                auto& clusterVariables = areaVariables[&area].clusterMap;
                int totalMntNumber = calculateNumberOfMaintenances(cluster, timeHorizon_);

                // loop per unit inside the cluster
                for (int unit = 0; unit < cluster.unitCount; ++unit)
                {
                    auto& unitVariables = clusterVariables[&cluster].unitMap;
                    // loop per maintenances per unit
                    for (int mnt = 0; mnt < totalMntNumber; ++mnt)
                    {
                        // add start[t][u][m] variables
                        unitVariables[unit].start.push_back(solver.MakeIntVar(
                          0.0,
                          1.0,
                          "S_[" + std::to_string(day + optSett.firstDay) + "]["
                            + cluster.getFullName().to<std::string>() + "." + std::to_string(unit)
                            + "][" + std::to_string(mnt) + "]"));

                        // add end[t][u][m] variables
                        unitVariables[unit].end.push_back(solver.MakeIntVar(
                          0.0,
                          1.0,
                          "E_[" + std::to_string(day + optSett.firstDay) + "]["
                            + cluster.getFullName().to<std::string>() + "." + std::to_string(unit)
                            + "][" + std::to_string(mnt) + "]"));
                    }
                }
            }
        }
    }
    return;
}

void OptimizedThermalGenerator::printAllVariables()
{
    for (MPVariable* const variable : solver.variables())
    {
        std::cout << "Variable: " << variable->name() << ", "
                  << "Lower bound: " << variable->lb() << ", "
                  << "Upper bound: " << variable->ub() << std::endl;
    }
    return;
}

} // namespace Antares::Solver::TSGenerator
