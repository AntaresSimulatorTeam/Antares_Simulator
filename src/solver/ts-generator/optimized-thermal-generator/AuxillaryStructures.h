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

// it is better to immediately calculate and populate structures
// that will store information about:
// for each area and each cluster:
// daily arrays with info about:
// daily MaxUnitOutput and
// daily UnitPowerCost
// so inside optimization problem we just retrieve these data
// not re-calculate them over and over again

struct DailyClusterDataPerCluster
{
    std::array<double, DAYS_PER_YEAR> maxPower;
    std::array<double, DAYS_PER_YEAR> unitCost;
    std::vector<int> daysSinceLastMnt;
};

struct DailyClusterDataPerArea
{
    // number of elements in the map is total number of cluster in area
    std::map<const Data::ThermalCluster*, DailyClusterDataPerCluster> clusterMap;
};

struct DailyClusterData
{
    // number of elements in the map is total number of areas in maintenance group
    std::map<const Data::Area*, DailyClusterDataPerArea> areaMap;
};

} // namespace Antares::Solver::TSGenerator
