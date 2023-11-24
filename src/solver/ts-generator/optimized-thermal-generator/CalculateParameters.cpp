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
        for (auto it = area.thermal.list.mapping.begin(); it != area.thermal.list.mapping.end();
             ++it)
        {
            const auto& cluster = *(it->second);
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
        auto tmpRenewable = calculateAverageRenewableTs(area);

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
        auto& areaVariables = maintenanceData.areaMap;
        areaVariables[&area] = AreaData();
        // loop per thermal clusters inside the area - fill in the structure
        for (auto it = area.thermal.list.mapping.begin(); it != area.thermal.list.mapping.end();
             ++it)
        {
            const auto& cluster = *(it->second);

            // we do not check if cluster.optimizeMaintenance = true here
            // we add all the clusters Power inside maintenance group
            if (!checkClusterExist(cluster))
                continue;

            auto& clusterVariables = areaVariables[&area].clusterMap;
            clusterVariables[&cluster] = ClusterData();
            clusterVariables[&cluster].maxPower = calculateMaxUnitOutput(cluster);
            clusterVariables[&cluster].avgCost = calculateAvrUnitDailyCost(cluster);

            clusterVariables[&cluster].numberOfMaintenances
              = calculateNumberOfMaintenances(cluster, timeHorizon_);
            clusterVariables[&cluster].averageMaintenanceDuration
              = calculateAverageMaintenanceDuration(cluster);

            // since we will be updating daysSinceLastMaintenance values
            // lets create a copy here - this is copy by value!
            clusterVariables[&cluster].daysSinceLastMnt = cluster.daysSinceLastMaintenance;
        }
    }
    return;
}

// calculate parameters methods - per cluster
int OptimizedThermalGenerator::calculateNumberOfMaintenances(const Data::ThermalCluster& cluster,
                                                             int timeHorizon)
{
    // timeHorizon cannot be 0. The whole maintenance group would be skipped if this happened
    // on the other hand interPoPeriod can be 0. So we say at least 2 maintenance if this happens
    if (cluster.interPoPeriod == 0)
    {
        logs.warning() << "Cluster: " << cluster.getFullName()
                       << "has interPoPeriod = 0. Number of maintenances for all units inside this "
                          "cluster will be set to 2";
        return minNumberOfMaintenances;
    }

    return std::max(timeHorizon / cluster.interPoPeriod, minNumberOfMaintenances); // floor
}

int OptimizedThermalGenerator::calculateAverageMaintenanceDuration(
  const Data::ThermalCluster& cluster)
{
    double sum = 0.0;
    for (std::size_t row = 0; row < DAYS_PER_YEAR; ++row)
    {
        sum += cluster.prepro->data[Data::PreproThermal::poDuration][row];
    }

    return sum / static_cast<double>(DAYS_PER_YEAR);
}

std::array<double, DAYS_PER_YEAR> OptimizedThermalGenerator::calculateMaxUnitOutput(
  const Data::ThermalCluster& cluster)
{
    std::array<double, DAYS_PER_YEAR> maxOutputDailyValues = {};
    std::array<double, HOURS_PER_YEAR> maxOutputHourlyValues = {};

    // transfer to array
    for (std::size_t row = 0; row < HOURS_PER_YEAR; ++row)
    {
        maxOutputHourlyValues[row]
          = cluster.modulation[Data::ThermalModulation::thermalModulationCapacity][row];
    }

    maxOutputDailyValues = calculateDailySums(maxOutputHourlyValues);
    // multiply by per unit power (nominal capacity)
    for (double& num : maxOutputDailyValues)
    {
        num *= cluster.nominalCapacity;
    }
    return maxOutputDailyValues;
}

std::array<double, DAYS_PER_YEAR> OptimizedThermalGenerator::calculateAvrUnitDailyCost(
  const Data::ThermalCluster& cluster)
{
    std::array<double, DAYS_PER_YEAR> avrCostDailyValues = {};
    std::array<double, HOURS_PER_YEAR> costHourlyValues = {};

    // transfer to array
    for (std::size_t row = 0; row < HOURS_PER_YEAR; ++row)
    {
        costHourlyValues[row]
          = cluster.modulation[Data::ThermalModulation::thermalModulationMarketBid][row];
    }

    avrCostDailyValues = calculateDailySums(costHourlyValues);
    // multiply by per unit/cluster market bid cost + average this on 24 hours
    for (double& num : avrCostDailyValues)
    {
        num *= cluster.marketBidCost / 24.0;
    }
    return avrCostDailyValues;
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

    return maintenanceData.areaMap[cluster.parentArea]
      .clusterMap[&cluster]
      .avgCost[dayOfTheYear(optimizationDay)];
}

double OptimizedThermalGenerator::getPowerOutput(const Data::ThermalCluster& cluster,
                                                 int optimizationDay)
{
    return maintenanceData.areaMap[cluster.parentArea]
      .clusterMap[&cluster]
      .maxPower[dayOfTheYear(optimizationDay)];
}

double OptimizedThermalGenerator::getResidualLoad(int optimizationDay)
{
    return residualLoadDailyValues_[dayOfTheYear(optimizationDay)];
}

int OptimizedThermalGenerator::getNumberOfMaintenances(const Data::ThermalCluster& cluster)
{
    return maintenanceData.areaMap[cluster.parentArea].clusterMap[&cluster].numberOfMaintenances;
}

int OptimizedThermalGenerator::getAverageMaintenanceDuration(const Data::ThermalCluster& cluster)
{
    return maintenanceData.areaMap[cluster.parentArea]
      .clusterMap[&cluster]
      .averageMaintenanceDuration;
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
    // s[tauLower-1][u][1] = 0

    // TODO CR27:
    // if the approach above breaks the solver
    // we force the start of the first maintenance to be after day=0
    // s[fixed = 0][u][1] = 0

    // TODO CR27:
    // we need to store cluster.daysSinceLastMaintenance[unitIndex]
    // somewhere locally - since we need to update the values after each timeStep_
    // and we do not want to change values inside "cluster" that will later be used for UI & txt

    if (unitIndex < cluster.daysSinceLastMaintenance.size())
    {
        return std::min(cluster.interPoPeriod
                          - maintenanceData.areaMap[cluster.parentArea]
                              .clusterMap[&cluster]
                              .daysSinceLastMnt[unitIndex]
                          - cluster.poWindows,
                        timeHorizon_ - 1);
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
    // latest start of the first maintenance of unit u (end of the window, must be positive):
    if (unitIndex < cluster.daysSinceLastMaintenance.size())
    {
        // this cannot be negative:
        // cluster.interPoPeriod - cluster.daysSinceLastMaintenance[unitIndex] - is always positive
        // or zero
        // cluster.poWindows is positive or zero
        // however we will make sure it does not surpass timeHorizon_ - 1 value

        return std::min(cluster.interPoPeriod
                          - maintenanceData.areaMap[cluster.parentArea]
                              .clusterMap[&cluster]
                              .daysSinceLastMnt[unitIndex]
                          + cluster.poWindows,
                        timeHorizon_ - 1);
    }
    else
    {
        logs.error() << "Cluster: " << cluster.getFullName()
                     << " does not have unit: " << unitIndex;
        return 0;
    }
}

// re-calculate parameters
void OptimizedThermalGenerator::reCalculateDaysSinceLastMnt(const OptProblemSettings& optSett)
{
    if (optSett.scenario == 0 && optSett.firstDay == 0)
        return;
    if (optSett.scenario != 0 && optSett.firstDay == 0)
    {
        // we are back in first step, but not first scenario we have messed up our values
        // we need to re-do
        // clusterVariables[&cluster].daysSinceLastMnt = cluster.daysSinceLastMaintenance;
        // for all areas and clusters
    }
    // TODO CR27: re-calculate days since last maintenance inputs if necessary
}

// auxillary functions - for parameter calculations
std::array<double, DAYS_PER_YEAR> OptimizedThermalGenerator::calculateDailySums(
  const std::array<double, HOURS_PER_YEAR>& hourlyValues)
{
    std::array<double, DAYS_PER_YEAR> dailyValues;
    auto hours_iter = hourlyValues.begin();

    for (double& day_sum : dailyValues)
    {
        day_sum = std::accumulate(hours_iter, hours_iter + 24, 0.0);
        hours_iter += 24;
    }

    return dailyValues;
}

std::array<double, HOURS_PER_YEAR> OptimizedThermalGenerator::calculateAverageTs(
  const Matrix<double>& tsValue,
  const Matrix<uint32_t>& tsNumbers)
{
    // define array
    std::array<double, HOURS_PER_YEAR> averageTs = {};
    // calculate sum
    for (std::size_t year = 0; year < tsNumbers.height; ++year)
    {
        for (std::size_t row = 0; row < HOURS_PER_YEAR; ++row)
        {
            averageTs[row] += tsValue[tsNumbers[0][year]][row];
        }
    }
    // calculate mean
    for (std::size_t row = 0; row < HOURS_PER_YEAR; ++row)
    {
        averageTs[row] = averageTs[row] / tsNumbers.height;
    }
    // return
    return averageTs;
}

bool OptimizedThermalGenerator::checkClusterExist(const Data::ThermalCluster& cluster)
{
    if (!cluster.prepro)
    {
        logs.error() << "Cluster: " << cluster.getFullName()
                     << ": The timeseries will not be regenerated. All data "
                        "related to the ts-generator for "
                     << "'thermal' have been released.";
        return false;
    }

    if (0 == cluster.unitCount || 0 == cluster.nominalCapacity)
    {
        return false;
    }
    return true;
}

int OptimizedThermalGenerator::dayOfTheYear(int optimizationDay)
{
    return optimizationDay % DAYS_PER_YEAR;
}

// calculate Average time-series functions
std::array<double, HOURS_PER_YEAR> OptimizedThermalGenerator::calculateAverageLoadTs(
  const Data::Area& area)
{
    // we assume ready-make TS - (pre-check exist for this!)
    const auto tsValues = area.load.series.timeSeries;
    const auto tsNumbers = area.load.series.timeseriesNumbers;
    return calculateAverageTs(tsValues, tsNumbers);
}

std::array<double, HOURS_PER_YEAR> OptimizedThermalGenerator::calculateAverageRorTs(
  const Data::Area& area)
{
    const auto tsValues = area.hydro.series->ror.timeSeries;
    auto tsNumbers = area.hydro.series->ror.timeseriesNumbers;
    return calculateAverageTs(tsValues, tsNumbers);
}

std::array<double, HOURS_PER_YEAR> OptimizedThermalGenerator::calculateAverageRenewableTsAggregated(
  const Data::Area& area)
{
    std::array<double, HOURS_PER_YEAR> averageTsSolar
      = calculateAverageTs(area.solar.series.timeSeries, area.solar.series.timeseriesNumbers);
    std::array<double, HOURS_PER_YEAR> averageTsWind
      = calculateAverageTs(area.wind.series.timeSeries, area.wind.series.timeseriesNumbers);

    std::array<double, HOURS_PER_YEAR> averageTsRenewable = {};
    for (std::size_t row = 0; row < HOURS_PER_YEAR; ++row)
    {
        averageTsRenewable[row] = (averageTsSolar[row] + averageTsWind[row]);
    }
    return averageTsRenewable;
}

std::array<double, HOURS_PER_YEAR> OptimizedThermalGenerator::calculateAverageRenewableTsClusters(
  const Data::Area& area)
{
    std::array<double, HOURS_PER_YEAR> averageTsRenewable = {};
    for (const auto& entryCluster : area.renewable.clusters)
    {
        auto& cluster = *entryCluster;
        // this is not even necessary - because area.renewable.clusters returns list of only
        // ENABLED clusters but let's keep it for now
        if (!cluster.enabled)
            continue;
        auto tmpArrayPerCluster
          = calculateAverageTs(cluster.series.timeSeries, cluster.series.timeseriesNumbers);
        for (std::size_t row = 0; row < HOURS_PER_YEAR; ++row)
        {
            if (cluster.tsMode == Data::RenewableCluster::productionFactor)
                averageTsRenewable[row]
                  += tmpArrayPerCluster[row] * cluster.unitCount * cluster.nominalCapacity;
            else
                averageTsRenewable[row] += tmpArrayPerCluster[row];
        }
    }
    return averageTsRenewable;
}

std::array<double, HOURS_PER_YEAR> OptimizedThermalGenerator::calculateAverageRenewableTs(
  const Data::Area& area)
{
    if (study.parameters.renewableGeneration.isAggregated())
    {
        return calculateAverageRenewableTsAggregated(area);
    }
    else // clusters it is
    {
        return calculateAverageRenewableTsClusters(area);
    }
}

} // namespace Antares::Solver::TSGenerator
