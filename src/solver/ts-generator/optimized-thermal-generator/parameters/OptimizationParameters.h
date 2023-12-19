//
// Created by milos on 14/11/23.
//

#pragma once

#include "../../randomized-thermal-generator/RandomizedGenerator.h"
#include "../main/OptimizedGenerator.h"

namespace Antares::Solver::TSGenerator
{

// it is better to immediately calculate and populate structure
// that will store information about clusters
// so inside optimization problem we just retrieve these data with getters
// not re-calculate them over and over again

// this structure stores cluster input data (optimization parameters)
// that stays the same during optimization - static inputs
// that are updated after each optimization - dynamic inputs
// the structure also temporary stores scenario results
// output/results for one scenario

struct StaticInputs
{
    // static input data
    // calculated once before all loops
    std::array<double, DAYS_PER_YEAR> maxPower;
    std::array<double, DAYS_PER_YEAR> avgCost;
    std::vector<int> numberOfMaintenancesFirstStep;
    int averageMaintenanceDuration;

    // for random generator
    double AP[366];
    double BP[366];
};

struct DynamicInputs
{
    // dynamic input data
    // re-calculated after each optimization time-step
    std::vector<int> daysSinceLastMaintenance;
    std::vector<int> numberOfMaintenances;
};

struct DynamicResults
{
    // scenario results
    // temporary store scenario results
    // before writing them to output
    // reset after each scenario
    std::vector<double> availableDailyPower;
};

struct ClusterData
{
    bool maintenanceEnabled;
    StaticInputs staticInputs;
    DynamicInputs dynamicInputs;
    DynamicResults dynamicResults;
};

class OptimizationParameters : public GeneratorTempData
{
    using OptimizationResults = std::vector<Unit>;

public:
    explicit OptimizationParameters(Data::Study& study,
                                    Data::MaintenanceGroup& maintenanceGroup,
                                    bool globalThermalTSgeneration,
                                    OptimizationVariables& vars,
                                    OptimizationResults& scenarioResults,
                                    Solver::Progression::Task& progr,
                                    IResultWriter& writer) :
     GeneratorTempData(study, progr, writer),
     maintenanceGroup_(maintenanceGroup),
     globalThermalTSgeneration_(globalThermalTSgeneration),
     modelingType_(study.parameters.renewableGeneration),
     vars_(vars),
     scenarioResults_(scenarioResults)
    {
    }

private:
    Data::MaintenanceGroup& maintenanceGroup_;
    const Data::Parameters::RenewableGeneration& modelingType_;
    const bool globalThermalTSgeneration_;
    const OptimizationVariables& vars_;
    OptimizationResults& scenarioResults_;

public:
    int scenarioLength_;
    int timeHorizon_;
    int timeHorizonFirstStep_;
    int timeStep_;
    double ensCost_;
    double spillCost_;
    std::array<double, DAYS_PER_YEAR> residualLoadDailyValues_;

    std::map<const Data::ThermalCluster*, ClusterData> clusterData;

    /* ===================CALCULATE-PARAMETERS-MAIN=================== */

    void allocateClusterData();
    void calculateNonDependantClusterData();
    void calculateResidualLoad();
    std::pair<double, double> calculateMaintenanceGroupENSandSpillageCost();
    int calculateTimeStep();
    int calculateTimeHorizon();
    void calculateDependantClusterData();
    void setMaintenanceGroupParameters();
    bool checkMaintenanceGroupParameters();

    /* ===================CALCULATE-PARAMETERS-PER CLUSTER/UNIT=================== */

    int calculateUnitEarliestStartOfFirstMaintenance(const Data::ThermalCluster& cluster,
                                                     uint unitIndex);
    int calculateUnitLatestStartOfFirstMaintenance(const Data::ThermalCluster& cluster,
                                                   uint unitIndex);
    std::vector<int> calculateNumberOfMaintenances(const Data::ThermalCluster& cluster);

    /* ===================GETTERS=================== */

    double getPowerCost(const Data::ThermalCluster& cluster, int optimizationDay);
    double getPowerOutput(const Data::ThermalCluster& cluster, int optimizationDay);
    double getResidualLoad(int optimizationDay);
    int getAverageMaintenanceDuration(const Data::ThermalCluster& cluster);
    int getAverageDurationBetweenMaintenances(const Data::ThermalCluster& cluster);
    int getNumberOfMaintenances(const Data::ThermalCluster& cluster, int unit);
    int getDaysSinceLastMaintenance(const Data::ThermalCluster& cluster, int unit);

    /* ===================POST-OPTIMIZATION=================== */

    /* ===================AFTER-EACH-TIME-STEP=================== */

    void postTimeStepOptimization(OptProblemSettings& optSett);
    void appendTimeStepResults(const OptProblemSettings& optSett);
    void reCalculateDaysSinceLastMnt(const OptProblemSettings& optSett);
    void reCalculateDaysSinceLastMnt(const OptProblemSettings& optSett, const Unit& unit);
    int reCalculateDaysSinceLastMnt(const OptProblemSettings& optSett,
                                    const Unit& unit,
                                    bool maintenanceHappened,
                                    int lastMaintenanceStart,
                                    int lastMaintenanceDuration);
    void reCalculateTimeHorizon();
    void reCalculateNumberOfMaintenances();
    bool checkTimeHorizon(OptProblemSettings& optSett);

    /* ===================AFTER-EACH-SCENARIO=================== */

    void postScenarioOptimization(OptProblemSettings& optSett);
    void calculateScenarioResults();
    void saveScenarioResults(const OptProblemSettings& optSett);
    void saveScenarioResults(int fromCol, int toCol, Data::ThermalCluster& cluster);
    void resetResultStorage();
    void reSetDaysSinceLastMnt();
    void reSetTimeHorizon();
    void reSetNumberOfMaintenances();

    /* ===================END-POST-OPTIMIZATION=================== */

    /* ===================PRINT-DEBUG-ONLY-TO-BE-REMOVED=================== */
    void printAllVariables(MPSolver& solver);
    void printObjectiveFunction(MPObjective* objective, MPSolver& solver);
    void printConstraints(MPSolver& solver);
    void printResults(OptProblemSettings& optSett, MPSolver& solver);
    void printProblemVarAndResults(OptProblemSettings& optSett, MPSolver& solver);
    void printMaintenances(OptProblemSettings& optSett);
    void printAvailability(OptProblemSettings& optSett);
};

} // namespace Antares::Solver::TSGenerator
