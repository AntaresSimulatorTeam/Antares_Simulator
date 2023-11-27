//
// Created by milos on 14/11/23.
//

#pragma once

#include "ortools/linear_solver/linear_solver.h"
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
    bool isFirstStep = true;
};

// this structure stores the data about optimization problem variables and results
struct Maintenances
{
    // number of elements in the vector is number of days in optimization problem
    std::vector<MPVariable*> start; // pointer to s[u][m][t] variables
    std::vector<MPVariable*> end;   // pointer to e[u][m][t] variables

    // methods
    int startDay(int limit) const;
    std::vector<int> getStartSolutionValues() const;
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

    // results
    // we will store results in the same structure
    // however we need separate instance of this struct
    // since the object that stores optimization variables is cleared after each timeStep
    // and the results need to be saved/appended for each timeStep

    // number of elements in the vector is number of maintenances
    // first element of the pair is start of the maintenance
    // second element of the pair is randomly drawn maintenance duration
    std::vector<std::pair<int, int>> maintenanceResults;
};

struct OptimizationProblemVariables
{
    // number of elements in the vector is number units (areas*cluster*units)
    std::vector<Unit> clusterUnits;

    // number of elements in the vector is number of days in optimization problem
    std::vector<MPVariable*> ens;   // pointers to Ens[t] variables
    std::vector<MPVariable*> spill; // pointers to Spill[t] variables
};

// it is better to immediately calculate and populate structure
// that will store information about clusters
// so inside optimization problem we just retrieve these data with getters
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
