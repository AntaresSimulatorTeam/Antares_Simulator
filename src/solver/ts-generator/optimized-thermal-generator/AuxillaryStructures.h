//
// Created by milos on 14/11/23.
//

#pragma once

#include "OptimizedGenerator.h"

using namespace operations_research;

namespace Antares::Solver::TSGenerator
{

class OptProblemSettings final
{
public:
    OptProblemSettings() = default;

    int firstDay;
    int lastDay;
};

struct OptimizationProblemVariablesPerUnit
{
    MPVariable* P = nullptr;; // index of P[t][u] variable

    // number of elements in the vector is total number of maintenances of unit
    std::vector<MPVariable*> start; // index of s[t][u][m] variable
    std::vector<MPVariable*> end;   // index of e[t][u][m] variable
};

struct OptimizationProblemVariablesPerCluster
{
    // number of elements in the vector is total number of units in cluster
    std::vector<OptimizationProblemVariablesPerUnit> unit;
};

struct OptimizationProblemVariablesPerArea
{
    // number of elements in the vector is total number of clusters in area
    std::vector<OptimizationProblemVariablesPerCluster> cluster;
};

struct OptimizationProblemVariablesPerDay
{
    MPVariable* Ens = nullptr;   // index of ENS[t] variable
    MPVariable* Spill = nullptr;; // index of Spillage[t] variable
    // number of elements in the vector is total number of areas in optimization problem
    std::vector<OptimizationProblemVariablesPerArea> area;
};
struct OptimizationProblemVariables
{
    // number of elements in the vector is total number of days in optimization problem
    std::vector<OptimizationProblemVariablesPerDay> day;
};

} // namespace Antares::Solver::TSGenerator
