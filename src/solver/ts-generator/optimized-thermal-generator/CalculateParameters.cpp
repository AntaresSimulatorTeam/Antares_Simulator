//
// Created by milos on 14/11/23.
//

#include "OptimizedGenerator.h"

namespace Antares::Solver::TSGenerator
{

// methods are ordered in the line of execution order
void OptimizedThermalGenerator::allocateClusterData()
{
    // loop all areas in maintenance group
    for (const auto& entryWeightMap : maintenanceGroup_)
    {
        const auto& area = *(entryWeightMap.first);
        // loop all thermal clusters inside the area
        for (const auto& clusterEntry : area.thermal.list.mapping)
        {
            const auto& cluster = *(clusterEntry.second);
            if (!checkClusterExist(cluster))
                continue;

            // create struct
            par.clusterData[&cluster] = ClusterData();
        }
    }
}

void OptimizedThermalGenerator::calculateNonDependantClusterData()
{
    for (auto& clusterEntry : par.clusterData)
    {
        auto& data = clusterEntry.second;
        const auto& cluster = *(clusterEntry.first);

        // static Inputs
        // this inputs wont be changed during optimization problem
        // and are important to calculate immediately
        // cause their values are used for calculating other parameters
        data.staticInputs.maxPower = calculateMaxUnitOutput(cluster);
        data.staticInputs.avgCost = calculateAvrUnitDailyCost(cluster);
        data.staticInputs.averageMaintenanceDuration = calculateAverageMaintenanceDuration(cluster);
        // static inputs for random generator
        prepareIndispoFromLaw(cluster.plannedLaw,
                              cluster.plannedVolatility,
                              data.staticInputs.AP,
                              data.staticInputs.BP,
                              cluster.prepro->data[Data::PreproThermal::poDuration]);

        // dynamic input
        // this input will be updated during optimization steps
        // and then re-sett
        // important to initialize it immediately
        // cause it is used for later parameter calculation
        data.dynamicInputs.daysSinceLastMaintenance
          = cluster.originalRandomlyGeneratedDaysSinceLastMaintenance;
    }
}

void OptimizedThermalGenerator::calculateResidualLoad()
{
    // create reference value arrays
    std::array<double, HOURS_PER_YEAR> refValueLoad = {};
    std::array<double, HOURS_PER_YEAR> refValueRor = {};
    std::array<double, HOURS_PER_YEAR> refValueRenewable = {};
    std::array<double, HOURS_PER_YEAR> refValue = {};

    // for phase II
    if (maintenanceGroup_.type() == Data::MaintenanceGroup::typeTimeserie)
    {
        // read user defined ts - userProvidedResidualLoadTS_ with getter - phase-II
        return;
    }

    // loop through the elements of weightMap weights_
    for (const auto& entryWeightMap : maintenanceGroup_)
    {
        const auto& area = *(entryWeightMap.first);
        const auto weights = (entryWeightMap.second);

        auto tmpLoad = calculateAverageLoadTs(area);
        auto tmpRor = calculateAverageRorTs(area);
        auto tmpRenewable = calculateAverageRenewableTs(study.parameters.renewableGeneration, area);

        for (std::size_t row = 0; row < HOURS_PER_YEAR; ++row)
        {
            refValueLoad[row] += tmpLoad[row] * weights.load;
            refValueRor[row] += tmpRor[row] * weights.ror;
            refValueRenewable[row] += tmpRenewable[row] * weights.renewable;
        }
    }
    // calculate reference value
    for (std::size_t row = 0; row < HOURS_PER_YEAR; ++row)
        refValue[row] = refValueLoad[row] - refValueRor[row] - refValueRenewable[row];
    // set ResidualLoadTS
    maintenanceGroup_.setUsedResidualLoadTS(refValue);
}

std::pair<double, double> OptimizedThermalGenerator::calculateMaintenanceGroupENSandSpillageCost()
{
    std::vector<int> ensVector = {};
    std::vector<int> spillageVector = {};
    for (const auto& entryWeightMap : maintenanceGroup_)
    {
        const auto& area = *(entryWeightMap.first);
        ensVector.push_back(area.thermal.unsuppliedEnergyCost);
        spillageVector.push_back(area.thermal.spilledEnergyCost);
    }

    // Using std::minmax_element to find min and max
    auto ens = std::min_element(ensVector.begin(), ensVector.end());
    auto spill = std::min_element(spillageVector.begin(), spillageVector.end());

    // Check if the vector is not empty
    if (ens != ensVector.end() && spill != spillageVector.end())
    {
        return std::make_pair(*ens, *spill);
    }

    return std::make_pair(0, 0);
}

int OptimizedThermalGenerator::calculateTimeStep()
{
    std::vector<int> averageDurationBetweenMaintenances = {};
    for (const auto& clusterEntry : par.clusterData)
    {
        const auto& cluster = *(clusterEntry.first);
        if (cluster.doWeGenerateTS(globalThermalTSgeneration_) && cluster.optimizeMaintenance)
        {
            averageDurationBetweenMaintenances.push_back(cluster.interPoPeriod);
        }
    }

    auto minIter = std::min_element(averageDurationBetweenMaintenances.begin(),
                                    averageDurationBetweenMaintenances.end());

    // Check if the vector is not empty
    if (minIter != averageDurationBetweenMaintenances.end())
    {
        return *minIter;
    }

    return 0;
}

int OptimizedThermalGenerator::calculateTimeHorizon()
{
    std::vector<int> timeHorizonVector = {};
    for (const auto& clusterEntry : par.clusterData)
    {
        const auto& cluster = *(clusterEntry.first);
        if (!(cluster.doWeGenerateTS(globalThermalTSgeneration_) && cluster.optimizeMaintenance))
            continue;
        for (int unit = 0; unit < cluster.unitCount; ++unit)
        {
            int value = 2 * getAverageDurationBetweenMaintenances(cluster)
                        + getAverageMaintenanceDuration(cluster)
                        - getDaysSinceLastMaintenance(cluster, unit);
            timeHorizonVector.push_back(value);
        }
    }

    auto maxIter = std::max_element(timeHorizonVector.begin(), timeHorizonVector.end());

    // Check if the vector is not empty
    if (maxIter != timeHorizonVector.end())
    {
        return *maxIter;
    }

    return 0;
}

// calculate parameters methods
void OptimizedThermalGenerator::setMaintenanceGroupParameters()
{
    // it is crucial that we allocateClusterData
    // and calculate NonDependantClusterData
    // since later methods expect this to be filled
    // and values available
    allocateClusterData();
    calculateNonDependantClusterData();
    //
    calculateResidualLoad();
    par.residualLoadDailyValues_ = calculateDailySums(maintenanceGroup_.getUsedResidualLoadTS());
    std::tie(par.ensCost_, par.spillCost_) = calculateMaintenanceGroupENSandSpillageCost();
    par.timeStep_ = calculateTimeStep();
    par.timeHorizon_ = calculateTimeHorizon();
    par.timeHorizonFirstStep_ = par.timeHorizon_;
    // calculateDependantClusterData
    // uses timeHorizon_ so it is important we calculate timeHorizon_ first
    calculateDependantClusterData();
}

void OptimizedThermalGenerator::calculateDependantClusterData()
{
    for (auto& clusterEntry : par.clusterData)
    {
        auto& data = clusterEntry.second;
        const auto& cluster = *(clusterEntry.first);

        /*
        calculateNumberOfMaintenances uses:
        averageMaintenanceDuration
        DaysSinceLastMaintenance
        timeHorizon
        averageDurationBetweenMaintenances - cluster.interPoPeriod - this is static always available
        so all these parameters need to be calculated before calling this method
        */

        // static input - wont be changed - used to reset dynamicInputs.numberOfMaintenances
        // after each scenario
        data.staticInputs.numberOfMaintenancesFirstStep = calculateNumberOfMaintenances(cluster);
        // dynamic inputs
        data.dynamicInputs.numberOfMaintenances = data.staticInputs.numberOfMaintenancesFirstStep;
    }
    return;
}

bool OptimizedThermalGenerator::checkMaintenanceGroupParameters()
{
    if (par.timeStep_ == 0)
    {
        logs.info() << "Maintenance group: " << maintenanceGroup_.name()
                    << ": The timeseries generation will be skiped:  timeStep = 0. It is possible "
                       "that the maintenance group has no clusters designated for maintenance "
                       "planning, or at least one cluster has interPoPeriod = 0";
        return false;
    }
    if (par.timeHorizon_ == 0)
    {
        logs.info()
          << "Maintenance group: " << maintenanceGroup_.name()
          << ": The timeseries generation will be skiped:  timeHorizon = 0. It is possible "
             "that the maintenance group has no clusters designated for maintenance "
             "planning, or all clusters have interPoPeriod = 0";
        return false;
    }
    // add some more check here if necessary!
    return true;
}

// Getters
double OptimizedThermalGenerator::getPowerCost(const Data::ThermalCluster& cluster,
                                               int optimizationDay)
{
    /*
    ** Unit cost can be directly set,
    ** Or calculated using Fuel Cost, Co2 cost, Fuel Eff and V&O Cost.

    ** In second case we also need information which year it is (to choose proper TS number, and
    also what hour it is)
    ** we need price per day (so averaging the hourly values)
    ** this is NOT calculated prior to the simulation - so if we only want to run ts-gen, we cannot
    get this info just yet
    ** using:
    ** getMarginalCost(uint serieIndex, uint hourInTheYear) or
    ** getMarketBidCost(uint hourInTheYear, uint year)
    ** TODO CR27: maybe for phase-II
    ** for now just disable this option but take into account the thermalModulationCost!!
    */

    if (cluster.costgeneration == Data::useCostTimeseries)
    {
        logs.warning()
          << "Cluster: " << cluster.getFullName()
          << " has Cost generation set to: Use cost timeseries. Option not suported yet. "
             "Cost set to zero.";
        return 0.;
    }

    return par.clusterData[&cluster].staticInputs.avgCost[dayOfTheYear(optimizationDay)];
}

double OptimizedThermalGenerator::getPowerOutput(const Data::ThermalCluster& cluster,
                                                 int optimizationDay)
{
    return par.clusterData[&cluster].staticInputs.maxPower[dayOfTheYear(optimizationDay)];
}

double OptimizedThermalGenerator::getResidualLoad(int optimizationDay)
{
    return par.residualLoadDailyValues_[dayOfTheYear(optimizationDay)];
}

int OptimizedThermalGenerator::getNumberOfMaintenances(const Data::ThermalCluster& cluster,
                                                       int unit)
{
    return par.clusterData[&cluster].dynamicInputs.numberOfMaintenances[unit];
}

int OptimizedThermalGenerator::getAverageMaintenanceDuration(const Data::ThermalCluster& cluster)
{
    return par.clusterData[&cluster].staticInputs.averageMaintenanceDuration;
}

int OptimizedThermalGenerator::getAverageDurationBetweenMaintenances(
  const Data::ThermalCluster& cluster)
{
    return cluster.interPoPeriod;
}

int OptimizedThermalGenerator::getDaysSinceLastMaintenance(const Data::ThermalCluster& cluster,
                                                           int unit)
{
    return par.clusterData[&cluster].dynamicInputs.daysSinceLastMaintenance[unit];
}

// calculate parameters methods - per cluster-Unit
int OptimizedThermalGenerator::calculateUnitEarliestStartOfFirstMaintenance(
  const Data::ThermalCluster& cluster,
  uint unitIndex)
{
    // earliest start of the first maintenance of unit u (beginning of the window, can be negative):
    // let it return negative value - if it returns negative value we wont implement constraint:
    // s[u][0][tauLower-1] = 0

    return (getAverageDurationBetweenMaintenances(cluster)
            - getDaysSinceLastMaintenance(cluster, unitIndex) - cluster.poWindows);
}

int OptimizedThermalGenerator::calculateUnitLatestStartOfFirstMaintenance(
  const Data::ThermalCluster& cluster,
  uint unitIndex)
{
    // latest start of the first maintenance of unit u, must be positive -
    // FIRST STEP ONLY!

    // cannot be negative: FIRST STEP ONLY
    // cluster.interPoPeriod -
    // cluster.originalRandomlyGeneratedDaysSinceLastMaintenance[unitIndex] - is always positive
    // or zero
    // cluster.poWindows is positive or zero
    // however we will make sure it does not surpass timeHorizon_ - 1 value
    // AFTER FIRST STEP it can go to negative value - so we will floor it to zero

    return std::min(
      std::max(0,
               getAverageDurationBetweenMaintenances(cluster)
                 - getDaysSinceLastMaintenance(cluster, unitIndex) + cluster.poWindows),
      par.timeHorizon_ - 1);
}

std::vector<int> OptimizedThermalGenerator::calculateNumberOfMaintenances(
  const Data::ThermalCluster& cluster)
{
    // getAverageMaintenanceDuration must be at least 1
    // so we do not need to check if div / 0

    std::vector<int> numberOfMaintenances;
    numberOfMaintenances.resize(cluster.unitCount);

    for (int unit = 0; unit != cluster.unitCount; ++unit)
    {
        int div = (par.timeHorizon_ + getDaysSinceLastMaintenance(cluster, unit)
                   - getAverageDurationBetweenMaintenances(cluster))
                  / (getAverageDurationBetweenMaintenances(cluster)
                     + getAverageMaintenanceDuration(cluster));
        numberOfMaintenances[unit] = std::max(
          1 + div, minNumberOfMaintenances); // TODO CR27: keep here min=2 did not see it in python
    }

    return numberOfMaintenances;
}

} // namespace Antares::Solver::TSGenerator
