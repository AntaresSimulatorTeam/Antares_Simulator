//
// Created by milos on 14/11/23.
//

#include "OptimizedGenerator.h"

namespace Antares::Solver::TSGenerator
{

// calculate parameters methods - per maintenance group
void OptimizedThermalGenerator::setMaintenanceGroupParameters()
{
    // timeHorizon, timeStep ENS and Spillage are defined per one MaintenanceGroup
    // Residual Load (or reference value) array is defined per one MaintenanceGroup
    calculateResidualLoad();
    residualLoadDailyValues_ = calculateDailySums(maintenanceGroup_.getUsedResidualLoadTS());
    std::tie(ensCost_, spillCost_) = calculateMaintenanceGroupENSandSpillageCost();
    std::tie(timeStep_, timeHorizon_) = calculateTimeHorizonAndStep();
    setClusterData();
}

bool OptimizedThermalGenerator::checkMaintenanceGroupParameters()
{
    if (timeStep_ == 0)
    {
        logs.info() << "Maintenance group: " << maintenanceGroup_.name()
                    << ": The timeseries generation will be skiped:  timeStep = 0. It is possible "
                       "that the maintenance group has no clusters designated for maintenance "
                       "planning, or at least one cluster has interPoPeriod = 0";
        return false;
    }
    if (timeHorizon_ == 0)
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

std::pair<int, int> OptimizedThermalGenerator::calculateTimeHorizonAndStep()
{
    std::vector<int> averageDurationBetweenMaintenances = {};
    for (const auto& entryWeightMap : maintenanceGroup_)
    {
        const auto& area = *(entryWeightMap.first);
        for (const auto& clusterEntry : area.thermal.list.mapping)
        {
            const auto& cluster = *(clusterEntry.second);
            if (checkClusterExist(cluster) && cluster.doWeGenerateTS(globalThermalTSgeneration_)
                && cluster.optimizeMaintenance)
            {
                averageDurationBetweenMaintenances.push_back(cluster.interPoPeriod);
            }
        }
    }

    // Using std::minmax_element to find min and max
    auto [minIter, maxIter] = std::minmax_element(averageDurationBetweenMaintenances.begin(),
                                                  averageDurationBetweenMaintenances.end());

    // Check if the vector is not empty
    if (minIter != averageDurationBetweenMaintenances.end()
        && maxIter != averageDurationBetweenMaintenances.end())
    {
        return std::make_pair(*minIter, 2 * (*maxIter));
    }

    return std::make_pair(0, 0);
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

void OptimizedThermalGenerator::setClusterData()
{
    for (const auto& entryWeightMap : maintenanceGroup_)
    {
        const auto& area = *(entryWeightMap.first);
        // loop per thermal clusters inside the area - fill in the structure
        for (const auto& clusterEntry : area.thermal.list.mapping)
        {
            const auto& cluster = *(clusterEntry.second);

            // we do not check if cluster.optimizeMaintenance = true here
            // we add all the clusters Power inside maintenance group
            if (!checkClusterExist(cluster))
                continue;

            maintenanceData[&cluster] = ClusterData();
            maintenanceData[&cluster].maxPower = calculateMaxUnitOutput(cluster);
            maintenanceData[&cluster].avgCost = calculateAvrUnitDailyCost(cluster);

            maintenanceData[&cluster].numberOfMaintenances
              = calculateNumberOfMaintenances(cluster, timeHorizon_);
            maintenanceData[&cluster].averageMaintenanceDuration
              = calculateAverageMaintenanceDuration(cluster);

            // since we will be updating daysSinceLastMaintenance values
            // lets create a copy here - this is copy by value!
            maintenanceData[&cluster].daysSinceLastMaintenance
              = cluster.originalRandomlyGeneratedDaysSinceLastMaintenance;

            // random generator
            prepareIndispoFromLaw(cluster.plannedLaw,
                                  cluster.plannedVolatility,
                                  maintenanceData[&cluster].AP,
                                  maintenanceData[&cluster].BP,
                                  cluster.prepro->data[Data::PreproThermal::poDuration]);
        }
    }
    return;
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

    return maintenanceData[&cluster].avgCost[dayOfTheYear(optimizationDay)];
}

double OptimizedThermalGenerator::getPowerOutput(const Data::ThermalCluster& cluster,
                                                 int optimizationDay)
{
    return maintenanceData[&cluster].maxPower[dayOfTheYear(optimizationDay)];
}

double OptimizedThermalGenerator::getResidualLoad(int optimizationDay)
{
    return residualLoadDailyValues_[dayOfTheYear(optimizationDay)];
}

int OptimizedThermalGenerator::getNumberOfMaintenances(const Data::ThermalCluster& cluster)
{
    return maintenanceData[&cluster].numberOfMaintenances;
}

int OptimizedThermalGenerator::getAverageMaintenanceDuration(const Data::ThermalCluster& cluster)
{
    return maintenanceData[&cluster].averageMaintenanceDuration;
}

int OptimizedThermalGenerator::getAverageDurationBetweenMaintenances(
  const Data::ThermalCluster& cluster)
{
    return cluster.interPoPeriod;
}

// calculate parameters methods - per cluster-Unit
int OptimizedThermalGenerator::calculateUnitEarliestStartOfFirstMaintenance(
  const Data::ThermalCluster& cluster,
  uint unitIndex)
{
    // earliest start of the first maintenance of unit u (beginning of the window, can be negative):
    // let it return negative value - if it returns negative value we wont implement constraint:
    // s[u][0][tauLower-1] = 0

    auto& daysSinceLastMaintenance = maintenanceData[&cluster].daysSinceLastMaintenance;

    if (unitIndex < daysSinceLastMaintenance.size())
    {
        return (cluster.interPoPeriod - daysSinceLastMaintenance[unitIndex] - cluster.poWindows);
    }
    else
    {
        logs.error() << "Cluster: " << cluster.getFullName()
                     << " does not have unit: " << unitIndex;
        return 0;
    }
}

int OptimizedThermalGenerator::calculateUnitLatestStartOfFirstMaintenance(
  const Data::ThermalCluster& cluster,
  uint unitIndex)
{
    // latest start of the first maintenance of unit u (end of the window, must be positive - 
    // FIRST STEP ONLY):

    auto& daysSinceLastMaintenance = maintenanceData[&cluster].daysSinceLastMaintenance;

    if (unitIndex < daysSinceLastMaintenance.size())
    {
        // this cannot be negative: FIRST STEP ONLY
        // cluster.interPoPeriod -
        // cluster.originalRandomlyGeneratedDaysSinceLastMaintenance[unitIndex] - is always positive
        // or zero
        // cluster.poWindows is positive or zero
        // however we will make sure it does not surpass timeHorizon_ - 1 value
        // AFTER FIRST STEP it can go to negative value - so we will floor it to zero

        return std::min(
          std::max(0,
                   cluster.interPoPeriod - daysSinceLastMaintenance[unitIndex] + cluster.poWindows),
          timeHorizon_ - 1);
    }
    else
    {
        logs.error() << "Cluster: " << cluster.getFullName()
                     << " does not have unit: " << unitIndex;
        return 0;
    }
}

} // namespace Antares::Solver::TSGenerator
