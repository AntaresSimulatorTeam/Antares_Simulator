//
// Created by milos on 10/11/23.
//

#pragma once

#include "../randomized-thermal-generator/RandomizedGenerator.h"
#include "../../../libs/antares/study/maintenance_planning/MaintenanceGroupRepository.h"
#include "../../../libs/antares/study/maintenance_planning/MaintenanceGroup.h"

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
    void calculateParameters();
    void buildProblemVariables();
    void setVariableBounds();
    void buildProblemConstraintsLHS();
    void buildProblemConstraintsRHS();
    void setProblemCost();
    void solveProblem();
    void allocateProblem(); // this one should be called in constructor. It basically resets all the
                            // vectors in PROBLEME_ANTARES_A_RESOUDRE for new opt problem.

    // optimization problem - methods - private
    void createOptimizationProblemPerCluster(const Data::Area& area, Data::ThermalCluster& cluster);

    // calculate parameters methods - per maintenance group
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

public:
    void run(); // calls private optimization problem construction methods

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
        nbThermalTimeseries = study.parameters.maintenancePlanning.getScenarioLength()
                              * study.parameters.maintenancePlanning.getScenarioNumber();
        // allocateProblem();
    }

    ~OptimizedThermalGenerator() = default;

    // optimization problem - methods - public
    void GenerateOptimizedThermalTimeSeries();
};

} // namespace Antares::Solver::TSGenerator