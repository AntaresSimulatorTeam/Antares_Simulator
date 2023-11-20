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
static const uint minNumberOfMaintenances = 2;

using namespace operations_research;

namespace Antares::Solver::TSGenerator
{

class OptimizedThermalGenerator : public GeneratorTempData
{
private:
    // optimization problem construction methods
    void buildProblemVariables(const OptProblemSettings& optSett);
    void countVariables();
    void allocateOptimizationProblemVariablesStruct(const OptProblemSettings& optSett);
    void buildEnsAndSpillageVariables(const OptProblemSettings& optSett);
    void buildUnitPowerOutputVariables(const OptProblemSettings& optSett);
    void buildStartEndMntVariables(const OptProblemSettings& optSett);
    void printAllVariables(); // for debug purpose only!

    void setVariableBounds();
    void setEnsAndSpillageBounds();
    void setUnitPowerOutputBounds();
    void setStartEndMntBounds();
    void setFirstMntStartBounds();
    void setAllMntMustStartBounds();

    void buildProblemConstraints();
    void buildLoadBalanceConstraints();
    void setStartEndMntLogicConstraints();
    void setMaxUnitOutputConstraints();

    void setProblemCost();
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

    // calculate parameters methods - per cluster
    uint calculateNumberOfMaintenances(const Data::ThermalCluster& cluster, uint timeHorizon);
    uint calculateAverageMaintenanceDuration(Data::ThermalCluster& cluster);
    std::array<double, DAYS_PER_YEAR> calculateMaxUnitOutput(Data::ThermalCluster& cluster);

    // calculate parameters methods - per cluster-Unit
    int calculateUnitEarliestStartOfFirstMaintenance(Data::ThermalCluster& cluster, uint unitIndex);
    int calculateUnitLatestStartOfFirstMaintenance(Data::ThermalCluster& cluster, uint unitIndex);

    // auxillary functions
    std::array<double, DAYS_PER_YEAR> calculateDailySums(
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