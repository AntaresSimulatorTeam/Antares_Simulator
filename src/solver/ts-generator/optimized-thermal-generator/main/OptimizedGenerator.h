//
// Created by milos on 10/11/23.
//

#pragma once

#include "ortools/linear_solver/linear_solver.h"
#include "../../randomized-thermal-generator/RandomizedGenerator.h"
#include "../../../../libs/antares/study/maintenance_planning/MaintenanceGroup.h"
#include "../support/SupportStructures.h"
#include "../support/SupportFunctions.h"
#include "../parameters/OptimizationParameters.h"
#include <antares/exception/AssertionError.hpp>

// static const std::string mntPlSolverName = "cbc";
static const int minNumberOfMaintenances = 2;
static const double solverDelta = 10e-4;

using namespace operations_research;

namespace Antares::Solver::TSGenerator
{

class OptimizedThermalGenerator : public GeneratorTempData
{
    using OptimizationResults = std::vector<Unit>;

private:
    /* ===================OPTIMIZATION=================== */

    // functions to build problem variables
    void buildProblemVariables(const OptProblemSettings& optSett);
    void buildEnsAndSpillageVariables(const OptProblemSettings& optSett);
    void buildUnitPowerOutputVariables(const OptProblemSettings& optSett);
    void buildUnitPowerOutputVariables(const OptProblemSettings& optSett,
                                       const Data::ThermalCluster& cluster);
    void buildUnitPowerOutputVariables(const OptProblemSettings& optSett,
                                       const Data::ThermalCluster& cluster,
                                       int unit);
    void buildStartEndMntVariables(const OptProblemSettings& optSett,
                                   const Data::ThermalCluster& cluster,
                                   int unit,
                                   Unit& unitRef);
    void buildStartVariables(const OptProblemSettings& optSett,
                             const Data ::ThermalCluster& cluster,
                             int unit,
                             Unit& unitRef,
                             int mnt);
    void buildEndVariables(const OptProblemSettings& optSett,
                           const Data ::ThermalCluster& cluster,
                           int unit,
                           Unit& unitRef,
                           int mnt);

    // functions to fix bounds of some variables
    void fixBounds();
    void fixBounds(const Unit& unit);
    void fixBounds(const Unit& unit, int averageMaintenanceDuration);
    void fixBoundsFirstMnt(const Unit& unit);
    void fixBoundsStartSecondMnt(const Unit& unit, int mnt);
    void fixBoundsMntEnd(const Unit& unit, int mnt, int averageMaintenanceDuration);

    // functions to build problem constraints
    void buildProblemConstraints(const OptProblemSettings& optSett);
    void setLoadBalanceConstraints(const OptProblemSettings& optSett);
    void setLoadBalanceConstraints(const OptProblemSettings& optSett, int& day);
    void insertEnsVars(MPConstraint* ct, int day);
    void insertSpillVars(MPConstraint* ct, int day);
    void insertPowerVars(MPConstraint* ct, int day);
    void insertPowerVars(MPConstraint* ct, int day, const Unit& unit);
    void setStartEndMntLogicConstraints(const OptProblemSettings& optSett);
    void setStartEndMntLogicConstraints(const OptProblemSettings& optSett, const Unit& unit);
    void setEndOfMaintenanceEventBasedOnAverageDurationOfMaintenanceEvent(
      const OptProblemSettings& optSett,
      const Unit& unit,
      int mnt);
    void setUpFollowingMaintenanceBasedOnAverageDurationBetweenMaintenanceEvents(
      const OptProblemSettings& optSett,
      const Unit& unit,
      int mnt);
    void setOnceStartIsSetToOneItWillBeOneUntilEndOfOptimizationTimeHorizon(
      const OptProblemSettings& optSett,
      const Unit& unit,
      int mnt);
    void setNextMaintenanceCanNotStartBeforePreviousMaintenance(const OptProblemSettings& optSett,
                                                                const Unit& cluster,
                                                                int mnt);
    void setMaxUnitOutputConstraints(const OptProblemSettings& optSett);
    void setMaxUnitOutputConstraints(const OptProblemSettings& optSett, int& day);
    void setMaxUnitOutputConstraints(const OptProblemSettings& optSett, int day, const Unit& unit);
    void insertStartSum(MPConstraint* ct, int day, const Unit& unit, double maxPower);
    void insertEndSum(MPConstraint* ct, int day, const Unit& cluster, double maxPower);

    // functions to set problem objective function
    void setProblemCost(const OptProblemSettings& optSett);
    void setProblemEnsCost(MPObjective* objective);
    void setProblemSpillCost(MPObjective* objective);
    void setProblemPowerCost(const OptProblemSettings& optSett, MPObjective* objective);
    void setProblemPowerCost(const OptProblemSettings& optSett,
                             MPObjective* objective,
                             const Unit& unit);

    // solve problem and check if optimal solution found
    bool solveProblem(OptProblemSettings& optSett);

    // reset problem and variable structure
    void resetProblem();

    /* ===================END-OPTIMIZATION=================== */

    /* ===================MAIN=================== */

    // Functions called in main method:
    void allocateWhereToWriteTs();
    bool runOptimizationProblem(OptProblemSettings& optSett);
    void writeTsResults();

    /* ===================END-MAIN=================== */

    /* ===================CLASS-VARIABLES=================== */

    // variables
    Data::MaintenanceGroup& maintenanceGroup_;
    bool globalThermalTSgeneration_;
    int scenarioLength_;
    int scenarioNumber_;

    OptimizationParameters par;
    OptimizationVariables vars;
    OptimizationResults scenarioResults;

    // MPSolver instance
    MPSolver solver;
    double solverInfinity;
    Solver::Progression::Task& progression_;
    /* ===================END-CLASS-VARIABLES=================== */

public:
    explicit OptimizedThermalGenerator(Data::Study& study,
                                       Data::MaintenanceGroup& maintenanceGroup,
                                       uint year,
                                       bool globalThermalTSgeneration,
                                       Solver::Progression::Task& progr,
                                       IResultWriter& writer) :
     GeneratorTempData(study, progr, writer),
     maintenanceGroup_(maintenanceGroup),
     progression_(progr),
     par(study, maintenanceGroup, globalThermalTSgeneration, vars, scenarioResults, progr, writer),
     solver(MPSolver("MaintenancePlanning", MPSolver::CBC_MIXED_INTEGER_PROGRAMMING))
    {
        currentYear = year;
        globalThermalTSgeneration_ = globalThermalTSgeneration;
        scenarioLength_ = study.parameters.maintenancePlanning.getScenarioLength();
        scenarioNumber_ = study.parameters.maintenancePlanning.getScenarioNumber();
        nbThermalTimeseries = scenarioLength_ * scenarioNumber_;
        par.scenarioLength_ = scenarioLength_;

        // Solver Settings
        // MP solver parameters / TODD CR27: do we change this -
        // I would keep it on default values for the time being

        // Access solver parameters
        MPSolverParameters params;
        // Set parameter values
        // params.SetIntegerParam(MPSolverParameters::SCALING, 0);
        // params.SetIntegerParam(MPSolverParameters::PRESOLVE, 0);

        // set solver infinity
        solverInfinity = solver.infinity();
    }

    ~OptimizedThermalGenerator() = default;

    // Main functions - loop per scenarios and
    // through the scenario length step by step
    // (moving window)
    void GenerateOptimizedThermalTimeSeries();
};

} // namespace Antares::Solver::TSGenerator