//
// Created by milos on 14/11/23.
//

#include <iostream>
#include <fstream>
#include <vector>

#include "OptimizedGenerator.h"

namespace Antares::Solver::TSGenerator
{
void OptimizedThermalGenerator::printProblemVarAndResults()
{
    printAllVariables();
    printConstraints();
    printObjectiveFunction(solver.MutableObjective());
    printResults();
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

void OptimizedThermalGenerator::printResults()
{
    std::vector<std::vector<double>> dataToPrint;

    // loop per day
    for (auto it = var.day.begin(); it != var.day.end(); ++it)
    {
        // row vector
        std::vector<double> RED;

        // ens and spill
        RED.push_back(it->Ens->solution_value());
        RED.push_back(it->Spill->solution_value());

        // powers + start/end
        for (const auto& cluster : it->areaMap)
        {
            for (const auto& unit : cluster.second.clusterMap)
            {
                for (const auto& perUnit : unit.second.unitMap)
                {
                    // powers
                    if (perUnit.P != nullptr)
                    {
                        RED.push_back(perUnit.P->solution_value());
                    }
                    // start and end
                    for (uint mnt = 0; mnt < perUnit.start.size(); ++mnt)
                    {
                        RED.push_back(perUnit.start[mnt]->solution_value());
                        RED.push_back(perUnit.end[mnt]->solution_value());
                    }
                }
            }
        }

        dataToPrint.push_back(RED);
    }

    printColumnToFile<double>(dataToPrint);

    return;
}

// Define the auxiliary function outside the class
template<typename T>
void printColumnToFile(const std::vector<std::vector<T>>& data)
{
    const std::string& filename = "/home/milos/Documents/RTEi/01-Antares/04-TestModels/CR27.csv";
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
