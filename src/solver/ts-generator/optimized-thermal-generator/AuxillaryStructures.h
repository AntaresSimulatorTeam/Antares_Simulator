//
// Created by milos on 14/11/23.
//

#pragma once

#include "OptimizedGenerator.h"
#include "../../../src/libs/antares/study/area/area.h"

using namespace operations_research;

namespace Antares::Solver::TSGenerator
{

// this class stores data about optimization problem settings
class OptProblemSettings final
{
public:
    OptProblemSettings() = default;

    int firstDay;
    int lastDay;
    int scenario;

    bool solved = true;
};

// this structure stores the data about optimization problem variables
struct Maintenances
{
    // number of elements in the vector is number of days in optimization problem
    std::vector<MPVariable*> start; // pointer to s[u][m][t] variables
    std::vector<MPVariable*> end;   // pointer to e[u][m][t] variables
};

struct Unit
{
    // inputs
    const Data::ThermalCluster* parentCluster;
    int index;
    bool createStartEndVariables;

    // solver variables

    // number of elements in the vector is number of days in optimization problem
    std::vector<MPVariable*> P; // pointers to P[u][t] variables

    // number of elements in the vector is total maintenances of unit
    std::vector<Maintenances> maintenances;
};

struct OptimizationProblemVariables
{
    // number of elements in the vector is number units (areas*cluster*units)
    std::vector<Unit> clusterUnits;

    // number of elements in the vector is number of days in optimization problem
    std::vector<MPVariable*> ens;   // pointers to Ens[t] variables
    std::vector<MPVariable*> spill; // pointers to Spill[t] variables
};

// it is better to immediately calculate and populate structures
// that will store information about clusters
// so inside optimization problem we just retrieve these data
// not re-calculate them over and over again

// this structure stores cluster input data (optimization parameters) 
// that stays the same during optimizationS
struct ClusterData
{
    std::array<double, DAYS_PER_YEAR> maxPower;
    std::array<double, DAYS_PER_YEAR> avgCost;
    std::vector<int> daysSinceLastMnt;
    int numberOfMaintenances;
    int averageMaintenanceDuration;
};

struct AreaData
{
    // number of elements in the map is total number of cluster in area
    std::map<const Data::ThermalCluster*, ClusterData> clusterMap;
};

struct MaintenanceData
{
    // number of elements in the map is total number of areas in maintenance group
    std::map<const Data::Area*, AreaData> areaMap;
};

} // namespace Antares::Solver::TSGenerator
