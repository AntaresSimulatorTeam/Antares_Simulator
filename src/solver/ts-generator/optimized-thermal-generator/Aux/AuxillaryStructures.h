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

struct Maintenances
{
    // number of elements in the vector is number of days in optimization problem
    std::vector<MPVariable*> start; // pointer to s[u][m][t] variables
    std::vector<MPVariable*> end;   // pointer to e[u][m][t] variables

    // methods
    int startDay(int limit) const;
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
    // since the object that stores optimization variables is cleared after each optimization
    // and the results need to be saved/appended for all optimizations (time steps)

    // number of elements in the vector is TOTAL number of maintenances
    // all the maintenances after all the time steps - not just one optimization
    // first element of the pair is start of the maintenance
    // second element of the pair is randomly drawn maintenance duration
    std::vector<std::pair<int, int>> maintenanceResults;
    // after all the maintenances are determined
    // we calculate UNIT availability - we do this once after all optimizations
    std::vector<double> availableDailyPower;

    // methods
    void calculateAvailableDailyPower(int tsCount);
};

// this structure stores the data about optimization problem variables and results
struct OptimizationVariables
{
    // number of elements in the vector is number units (areas*cluster*units)
    std::vector<Unit> clusterUnits;

    // number of elements in the vector is number of days in optimization problem
    std::vector<MPVariable*> ens;   // pointers to Ens[t] variables
    std::vector<MPVariable*> spill; // pointers to Spill[t] variables
};

} // namespace Antares::Solver::TSGenerator
