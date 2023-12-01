//
// Created by milos on 14/11/23.
//

#pragma once

#include "../../randomized-thermal-generator/RandomizedGenerator.h"
#include "../Main/OptimizedGenerator.h"

namespace Antares::Solver::TSGenerator
{

// it is better to immediately calculate and populate structure
// that will store information about clusters
// so inside optimization problem we just retrieve these data with getters
// not re-calculate them over and over again

// this structure stores cluster input data (optimization parameters)
// that stays the same during optimizationS
// also temporary stores cluster output/results for one scenario

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
    StaticInputs staticInputs;
    DynamicInputs dynamicInputs;
    DynamicResults dynamicResults;
};

class OptimizationParameters : public GeneratorTempData
{
public:
    explicit OptimizationParameters(Data::Study& study,
                                    Data::MaintenanceGroup& maintenanceGroup,
                                    bool globalThermalTSgeneration,
                                    Solver::Progression::Task& progr,
                                    IResultWriter& writer) :
     GeneratorTempData(study, progr, writer),
     maintenanceGroup_(maintenanceGroup),
     modelingType_(study.parameters.renewableGeneration)
    {
        globalThermalTSgeneration_ = globalThermalTSgeneration;
    }

private:
    Data::MaintenanceGroup& maintenanceGroup_;
    Data::Parameters::RenewableGeneration& modelingType_;
    bool globalThermalTSgeneration_;

public:
    int timeHorizon_;
    int timeHorizonFirstStep_;
    int timeStep_;
    double ensCost_;
    double spillCost_;
    std::array<double, DAYS_PER_YEAR> residualLoadDailyValues_;

    std::map<const Data::ThermalCluster*, ClusterData> clusterData;

    // Calculate parameters methods - per maintenance group
    void allocateClusterData();
    void calculateNonDependantClusterData();
    void calculateResidualLoad();
    std::pair<double, double> calculateMaintenanceGroupENSandSpillageCost();
    int calculateTimeStep();
    int calculateTimeHorizon();
    void calculateDependantClusterData();
    void setMaintenanceGroupParameters();
    bool checkMaintenanceGroupParameters();

    // getters
    double getPowerCost(const Data::ThermalCluster& cluster, int optimizationDay);
    double getPowerOutput(const Data::ThermalCluster& cluster, int optimizationDay);
    double getResidualLoad(int optimizationDay);
    int getAverageMaintenanceDuration(const Data::ThermalCluster& cluster);
    int getAverageDurationBetweenMaintenances(const Data::ThermalCluster& cluster);
    int getNumberOfMaintenances(const Data::ThermalCluster& cluster, int unit);
    int getDaysSinceLastMaintenance(const Data::ThermalCluster& cluster, int unit);
    // re-calculate parameters methods - per cluster-Unit
    int calculateUnitEarliestStartOfFirstMaintenance(const Data::ThermalCluster& cluster,
                                                     uint unitIndex);
    int calculateUnitLatestStartOfFirstMaintenance(const Data::ThermalCluster& cluster,
                                                   uint unitIndex);
    std::vector<int> calculateNumberOfMaintenances(const Data::ThermalCluster& cluster);
};

} // namespace Antares::Solver::TSGenerator
