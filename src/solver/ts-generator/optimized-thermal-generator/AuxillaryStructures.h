//
// Created by milos on 14/11/23.
//

namespace Antares::Solver::TSGenerator
{

class OptProblemSettings final
{
public:
    OptProblemSettings() = default;

    int optProblemStartTime;
    int optProblemEndTime;
};

struct OptimizationProblemVariableIndexesPerUnit
{
    int P; // index of P[t][u] variable

    // number of elements in the vector is total number of maintenances of unit
    std::vector<int> start; // index of s[t][u][m] variable
    std::vector<int> end;   // index of e[t][u][m] variable
};

struct OptimizationProblemVariableIndexesPerCluster
{
    // number of elements in the vector is total number of units in cluster
    std::vector<OptimizationProblemVariableIndexesPerUnit> unit;
};

struct OptimizationProblemVariableIndexesPerArea
{
    // number of elements in the vector is total number of clusters in area
    std::vector<OptimizationProblemVariableIndexesPerCluster> cluster;
};

struct OptimizationProblemVariableIndexesPerDay
{
    int Ens;   // index of ENS[t] variable
    int Spill; // index of Spillage[t] variable
    // number of elements in the vector is total number of areas in optimization problem
    std::vector<OptimizationProblemVariableIndexesPerArea> area;
};
struct OptimizationProblemVariableIndexes
{
    // number of elements in the vector is total number of days in optimization problem
    std::vector<OptimizationProblemVariableIndexesPerDay> day;
};

} // namespace Antares::Solver::TSGenerator
