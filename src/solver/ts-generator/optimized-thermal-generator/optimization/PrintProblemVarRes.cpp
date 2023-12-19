//
// Created by milos on 14/11/23.
//

#include <iostream>
#include <fstream>
#include <vector>
#include <string.h>
#include <algorithm>

#include "../main/OptimizedGenerator.h"

namespace Antares::Solver::TSGenerator
{
void OptimizationParameters::printProblemVarAndResults(OptProblemSettings& optSett, MPSolver& solver)
{
    printAllVariables(solver);
    printConstraints(solver);
    printObjectiveFunction(solver.MutableObjective(), solver);
    printResults(optSett, solver);
}

void OptimizationParameters::printAllVariables(MPSolver& solver)
{
    std::vector<std::vector<std::string>> dataToPrint;

    for (MPVariable* const variable : solver.variables())
    {
        std::vector<std::string> RED;
        std::cout << "Variable: " << variable->name() << ", "
                  << "Lower bound: " << variable->lb() << ", "
                  << "Upper bound: " << variable->ub() << std::endl;
        RED.push_back(variable->name());
        RED.push_back(std::to_string(variable->lb()));
        RED.push_back(std::to_string(variable->ub()));
        dataToPrint.push_back(RED);
    }

    std::cout << "total number of variables is: " << solver.NumVariables() << std::endl;
    printColumnToFile<std::string>(
      dataToPrint, "/home/milos/Documents/RTEi/01-Antares/04-TestModels/REFACTOR-CR27-Vars.csv");
    return;
}

void OptimizationParameters::printConstraints(MPSolver& solver)
{
    const int num_constraints = solver.NumConstraints();
    std::vector<std::vector<std::string>> dataToPrint;
    for (int i = 0; i < num_constraints; ++i)
    {
        std::vector<std::string> RED;
        const MPConstraint* const constraint = solver.constraint(i);
        std::cout << "**** Constraint " << i + 1 << " ****" << std::endl;
        std::cout << "Name: " << constraint->name() << std::endl;
        std::cout << "Lower Bound: " << constraint->lb() << std::endl;
        std::cout << "Upper Bound: " << constraint->ub() << std::endl;

        RED.push_back(constraint->name());
        RED.push_back(std::to_string(constraint->lb()));
        RED.push_back(std::to_string(constraint->ub()));

        std::vector<std::string> RED_SORT;
        for (const auto& term : constraint->terms())
        {
            const MPVariable* const var = term.first;
            const double coefficient = term.second;
            std::cout << var->name() << ": " << coefficient << std::endl;
            RED_SORT.push_back(var->name() + "->" + std::to_string(coefficient));
        }

        // sort sub vector
        std::sort(RED_SORT.begin(), RED_SORT.end());
        // insert it in main vector
        RED.insert(RED.end(), RED_SORT.begin(), RED_SORT.end());
        // end of row
        dataToPrint.push_back(RED);
        std::cout << "------------------------" << std::endl;
    }

    printColumnToFile<std::string>(
      dataToPrint,
      "/home/milos/Documents/RTEi/01-Antares/04-TestModels/REFACTOR-CR27-Constraints.csv");
}

void OptimizationParameters::printObjectiveFunction(MPObjective* objective, MPSolver& solver)
{
    std::vector<std::vector<std::string>> dataToPrint;
    for (MPVariable* variable : solver.variables())
    {
        std::cout << variable->name() << ": " << objective->GetCoefficient(variable) << std::endl;
        std::vector<std::string> RED;
        RED.push_back(variable->name());
        RED.push_back(std::to_string(objective->GetCoefficient(variable)));
        dataToPrint.push_back(RED);
    }
    std::cout << std::endl;
    printColumnToFile<std::string>(
      dataToPrint,
      "/home/milos/Documents/RTEi/01-Antares/04-TestModels/REFACTOR-CR27-Objective.csv");
    return;
}

void OptimizationParameters::printResults(OptProblemSettings& optSett, MPSolver& solver)
{
    std::vector<std::vector<double>> dataToPrint;

    // loop per day
    for (auto day = 0; day != vars_.ens.size(); ++day)
    {
        // row vector
        std::vector<double> RED;

        // ens and spill
        RED.push_back(vars_.ens[day]->solution_value());
        RED.push_back(vars_.spill[day]->solution_value());

        // powers + start/end
        for (const auto& unit : vars_.clusterUnits)
        {
            // powers
            if (unit.P[day] != nullptr)
            {
                RED.push_back(unit.P[day]->solution_value());
            }
            // start and end
            for (uint mnt = 0; mnt < unit.maintenances.size(); ++mnt)
            {
                RED.push_back(unit.maintenances[mnt].start[day]->solution_value());
                RED.push_back(unit.maintenances[mnt].end[day]->solution_value());
            }
        }

        dataToPrint.push_back(RED);
    }

    printf("Optimal objective value = %.2f\n", solver.MutableObjective()->Value());

    std::string fileName = "/home/milos/Documents/RTEi/01-Antares/04-TestModels/"
                           + std::to_string(optSett.firstDay) + "-Results.csv";

    printColumnToFile<double>(dataToPrint, fileName);

    return;
}

void OptimizationParameters::printMaintenances(OptProblemSettings& optSett)
{
    std::vector<std::vector<int>> dataToPrint;
    for (auto& unit : scenarioResults_)
    {
        std::vector<int> RED;
        RED.push_back(9999);
        RED.push_back(9999);
        dataToPrint.push_back(RED);
        for (auto& mnt : unit.maintenanceResults)
        {
            RED.clear();
            RED.push_back(mnt.first);
            RED.push_back(mnt.second);
            dataToPrint.push_back(RED);
        }
    }

    std::string fileName = "/home/milos/Documents/RTEi/01-Antares/04-TestModels/"
                           + std::to_string(optSett.firstDay) + "-Maintenances.csv";

    printColumnToFile<int>(dataToPrint, fileName);
}

void OptimizationParameters::printAvailability(OptProblemSettings& optSett)
{
    std::vector<std::vector<double>> dataToPrint;

    for (int day = 0; day != scenarioLength_ * 365; ++day)
    {
        std::vector<double> RED;
        for (auto& cluster : clusterData)
        {
            RED.push_back(cluster.second.dynamicResults.availableDailyPower[day]);
        }

        dataToPrint.push_back(RED);
    }

    std::string fileName = "/home/milos/Documents/RTEi/01-Antares/04-TestModels/"
                           + std::to_string(optSett.firstDay) + "-Availability.csv";

    printColumnToFile<double>(dataToPrint, fileName);
}

// Define the auxiliary function outside the class
template<typename T>
void printColumnToFile(const std::vector<std::vector<T>>& data, const std::string& filename)
{
    std::ofstream outFile(filename);
    if (outFile.is_open())
    {
        for (size_t row = 0; row < data.size(); ++row)
        {
            for (size_t col = 0; col < data[row].size(); ++col)
            {
                outFile << data[row][col];
                outFile << ",";
            }
            outFile << std::endl;
        }
        outFile.close();
    }
    else
    {
        std::cerr << "Unable to open file: " << filename << std::endl;
    }
}

} // namespace Antares::Solver::TSGenerator