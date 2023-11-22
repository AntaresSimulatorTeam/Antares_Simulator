//
// Created by milos on 10/11/23.
//

#pragma once

#include "ortools/linear_solver/linear_solver.h"
#include "../randomized-thermal-generator/RandomizedGenerator.h"
#include "../../../libs/antares/study/maintenance_planning/MaintenanceGroup.h"
#include "AuxillaryStructures.h"
#include <antares/exception/AssertionError.hpp>

// static const std::string mntPlSolverName = "cbc";
static const int minNumberOfMaintenances = 2;

using namespace operations_research;

namespace Antares::Solver::TSGenerator
{

class OptimizedThermalGenerator : public GeneratorTempData
{
private:
    // optimization problem construction methods

    void countVariables();

    void allocateVarStruct();
    void allocateVarStruct(int day);
    void allocateVarStruct(int day, const Data::Area& area);
    void allocateVarStruct(int day, const Data::ThermalCluster& cluster);

    void buildProblemVariables(const OptProblemSettings& optSett);

    void buildEnsAndSpillageVariables(const OptProblemSettings& optSett);

    void buildUnitPowerOutputVariables(const OptProblemSettings& optSett);
    void buildUnitPowerOutputVariables(const OptProblemSettings& optSett, int day);
    void buildUnitPowerOutputVariables(const OptProblemSettings& optSett,
                                       int day,
                                       const Data::Area& area);
    void buildUnitPowerOutputVariables(const OptProblemSettings& optSett,
                                       int day,
                                       const Data::ThermalCluster& cluster);

    void buildStartEndMntVariables(const OptProblemSettings& optSett);
    void buildStartEndMntVariables(const OptProblemSettings& optSett, int day);
    void buildStartEndMntVariables(const OptProblemSettings& optSett,
                                   int day,
                                   const Data ::Area& area);
    void buildStartEndMntVariables(const OptProblemSettings& optSett,
                                   int day,
                                   const Data ::ThermalCluster& cluster);
    void buildStartEndMntVariables(const OptProblemSettings& optSett,
                                   int day,
                                   const Data ::ThermalCluster& cluster,
                                   int unit,
                                   int totalMntNumber);

    void printAllVariables(); // for debug purpose only!

    void setVariableBounds();

    void fixBounds();
    void fixBounds(const Data::Area& area);
    void fixBounds(const Data::ThermalCluster& cluster);
    void fixBoundsStartFirstMnt(const Data::ThermalCluster& cluster, int unit);
    void fixBoundsEndOfMnt(const Data::ThermalCluster& cluster, int unit, int totalMntNum);

    void buildProblemConstraints();
    void buildLoadBalanceConstraints();
    void setStartEndMntLogicConstraints();
    void setMaxUnitOutputConstraints();

    void setProblemCost(const OptProblemSettings& optSett);
    void setProblemEnsCost(MPObjective* objective);
    void setProblemSpillCost(MPObjective* objective);
    void setProblemPowerCost(const OptProblemSettings& optSett, MPObjective* objective);
    void setProblemPowerCost(const OptProblemSettings& optSett, MPObjective* objective, int day);
    void setProblemPowerCost(const OptProblemSettings& optSett,
                             MPObjective* objective,
                             int day,
                             const Data::Area& area);
    void setProblemPowerCost(const OptProblemSettings& optSett,
                             MPObjective* objective,
                             int day,
                             const Data::Area& area,
                             const Data::ThermalCluster& cluster);
    void setProblemPowerCost(MPObjective* objective,
                             int day,
                             const Data::Area& area,
                             const Data::ThermalCluster& cluster,
                             int unitIndex,
                             double powerCost);

    void printObjectiveFunction(MPObjective* objective);

    void solveProblem();
    void resetProblem();

    void runOptimizationProblem(const OptProblemSettings& optSett);

    // optimization problem - methods - private
    void createOptimizationProblemPerGroup(const OptProblemSettings& optSett);
    void createOptimizationProblemPerCluster(const Data::Area& area, Data::ThermalCluster& cluster);

    // calculate parameters methods - per maintenance group
    void setMaintenanceGroupParameters();
    bool checkMaintenanceGroupParameters();
    std::pair<int, int> calculateTimeHorizonAndStep();
    std::pair<double, double> calculateMaintenanceGroupENSandSpillageCost();
    void calculateResidualLoad();
    void setClusterDailyValues();

    // calculate parameters methods - per cluster
    int calculateNumberOfMaintenances(const Data::ThermalCluster& cluster, int timeHorizon);
    uint calculateAverageMaintenanceDuration(Data::ThermalCluster& cluster);
    static std::array<double, DAYS_PER_YEAR> calculateMaxUnitOutput(const Data::ThermalCluster& cluster);
    static std::array<double, DAYS_PER_YEAR> calculateAvrUnitDailyCost(const Data::ThermalCluster& cluster);
    // getters
    double getUnitPowerCost(const Data::ThermalCluster& cluster, int optimizationDay);
    double getUnitPowerOutput(const Data::ThermalCluster& cluster, int optimizationDay);

    // calculate parameters methods - per cluster-Unit
    int calculateUnitEarliestStartOfFirstMaintenance(const Data::ThermalCluster& cluster,
                                                     uint unitIndex);
    int calculateUnitLatestStartOfFirstMaintenance(const Data::ThermalCluster& cluster,
                                                   uint unitIndex);

    // auxillary functions
    static std::array<double, DAYS_PER_YEAR> calculateDailySums(
      const std::array<double, HOURS_PER_YEAR>& hourlyValues);
    std::array<double, HOURS_PER_YEAR> calculateAverageTs(const Matrix<double>& tsValue,
                                                          const Matrix<uint32_t>& tsNumbers);
    bool checkClusterExist(const Data::ThermalCluster& cluster);
    int dayOfTheYear(int optimizationDay);

    // calculate Average time-series functions
    std::array<double, HOURS_PER_YEAR> calculateAverageLoadTs(const Data::Area& area);
    std::array<double, HOURS_PER_YEAR> calculateAverageRorTs(const Data::Area& area);
    std::array<double, HOURS_PER_YEAR> calculateAverageRenewableTs(const Data::Area& area);
    std::array<double, HOURS_PER_YEAR> calculateAverageRenewableTsAggregated(
      const Data::Area& area);
    std::array<double, HOURS_PER_YEAR> calculateAverageRenewableTsClusters(const Data::Area& area);

    // variables
    Data::MaintenanceGroup& maintenanceGroup_;
    bool globalThermalTSgeneration_;
    int scenarioLength_;
    int scenarioNumber_;
    int timeHorizon_;
    int timeStep_;
    double ensCost_;
    double spillCost_;
    std::array<double, DAYS_PER_YEAR> residualLoadDailyValues_;
    DailyClusterData dailyClusterData;
    OptimizationProblemVariables var;

    // MPSolver instance
    MPSolver solver;
    double infinity;

public:
    explicit OptimizedThermalGenerator(Data::Study& study,
                                       Data::MaintenanceGroup& maintenanceGroup,
                                       uint year,
                                       bool globalThermalTSgeneration,
                                       Solver::Progression::Task& progr,
                                       IResultWriter& writer) :
     GeneratorTempData(study, progr, writer),
     maintenanceGroup_(maintenanceGroup),
     solver(MPSolver("MaintenancePlanning", MPSolver::CBC_MIXED_INTEGER_PROGRAMMING))
    {
        currentYear = year;
        globalThermalTSgeneration_ = globalThermalTSgeneration;
        scenarioLength_ = study.parameters.maintenancePlanning.getScenarioLength();
        scenarioNumber_ = study.parameters.maintenancePlanning.getScenarioNumber();
        nbThermalTimeseries = scenarioLength_ * scenarioNumber_;

        // Solver Settings
        // MP solver parameters / TODD CR27: do we change this -
        // I would keep it on default values for the time being
        
        // Access solver parameters
        MPSolverParameters params;
        // Set parameter values
        // params.SetIntegerParam(MPSolverParameters::SCALING, 0);
        // params.SetIntegerParam(MPSolverParameters::PRESOLVE, 0);
        
        // set solver infinity
        infinity = solver.infinity();
    }

    ~OptimizedThermalGenerator() = default;

    // optimization problem - methods - public
    void GenerateOptimizedThermalTimeSeries();
};

} // namespace Antares::Solver::TSGenerator