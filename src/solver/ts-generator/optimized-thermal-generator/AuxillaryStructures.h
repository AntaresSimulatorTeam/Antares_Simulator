//
// Created by milos on 14/11/23.
//

#pragma once

#include "OptimizedGenerator.h"
#include "../../../src/libs/antares/study/area/area.h"

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
    std::vector<OptimizationProblemVariablesPerUnit> unitMap;
};

struct OptimizationProblemVariablesPerArea
{
    // number of elements in the map is total number of clusters in area
    std::map<const Data::ThermalCluster*, OptimizationProblemVariablesPerCluster> clusterMap;
};

struct OptimizationProblemVariablesPerDay
{
    MPVariable* Ens = nullptr;   // index of ENS[t] variable
    MPVariable* Spill = nullptr;; // index of Spillage[t] variable
    // number of elements in the map is total number of areas in optimization problem
    std::map<const Data::Area*, OptimizationProblemVariablesPerArea> areaMap;
};
struct OptimizationProblemVariables
{
    // number of elements in the vector is total number of days in optimization problem
    std::vector<OptimizationProblemVariablesPerDay> day;
};

} // namespace Antares::Solver::TSGenerator
