//
// Created by milos on 10/11/23.
//

#pragma once

#include "../randomized-thermal-generator/RandomizedGenerator.h"
//#include "../../../libs/antares/study/maintenance_planning/MaintenanceGroupRepository.h"
#include "../../../libs/antares/study/maintenance_planning/MaintenanceGroup.h"
#include "AuxillaryStructures.h"

namespace Antares::Solver::TSGenerator
{

class OptimizedThermalGenerator : public GeneratorTempData
{
private:
    /*
    in adq-patch we re-used existing structure(s) for helping us define an optimization problem ->
    like: struct CORRESPONDANCES_DES_VARIABLES.
    unfortunately here we have to create our own help-structure if we need one
    Question: can we still use struct PROBLEME_ANTARES_A_RESOUDRE ?! - no we use MPSolver class
    */

    // define here variables/structures that will help us build optimization problem

    // optimization problem construction methods
    void buildProblemVariables();
    void countVariables();
    void buildEnsAndSpillageVariables();
    void buildUnitPowerOutputVariables();
    void buildStartEndMntVariables();

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
    // void buildProblemConstraintsRHS(); // let's do LHS & RHS in one go. Easier!?

    void setProblemCost();
    void solveProblem();
    void resetProblem();

    void runOptimizationProblem();


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
    uint calculateNumberOfMaintenances(Data::ThermalCluster& cluster, uint timeHorizon);
    uint calculateAverageMaintenanceDuration(Data::ThermalCluster& cluster);
    std::array<double, DAYS_PER_YEAR> calculateMaxUnitOutput(Data::ThermalCluster& cluster);

    // calculate parameters methods - per cluster-Unit
    uint calculateUnitEarliestStartOfFirstMaintenance(Data::ThermalCluster& cluster,
                                                      int avrMntDuration,
                                                      uint unitIndex);
    uint calculateUnitLatestStartOfFirstMaintenance(Data::ThermalCluster& cluster,
                                                    int avrMntDuration,
                                                    uint unitIndex);

    // auxillary functions
    std::array<double, DAYS_PER_YEAR> calculateDailySums(
      const std::array<double, HOURS_PER_YEAR>& hourlyValues);
    std::array<double, HOURS_PER_YEAR> calculateAverageTs(const Matrix<double>& tsValue,
                                                          const Matrix<uint32_t>& tsNumbers);
    bool checkClusterData(const Data::Area& area, Data::ThermalCluster& cluster);
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

public:
    explicit OptimizedThermalGenerator(Data::Study& study,
                                       Data::MaintenanceGroup& maintenanceGroup,
                                       uint year,
                                       bool globalThermalTSgeneration,
                                       Solver::Progression::Task& progr,
                                       IResultWriter& writer) :
     GeneratorTempData(study, progr, writer), maintenanceGroup_(maintenanceGroup)
    {
        currentYear = year;
        globalThermalTSgeneration_ = globalThermalTSgeneration;
        scenarioLength_ = study.parameters.maintenancePlanning.getScenarioLength();
        scenarioNumber_ = study.parameters.maintenancePlanning.getScenarioNumber();
        nbThermalTimeseries = scenarioLength_ * scenarioNumber_;
        // allocateProblem();
    }

    ~OptimizedThermalGenerator() = default;

    // optimization problem - methods - public
    void GenerateOptimizedThermalTimeSeries();
};

} // namespace Antares::Solver::TSGenerator