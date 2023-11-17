//
// Created by milos on 10/11/23.
//

#include "OptimizedGenerator.h"

#include <numeric>
#include <algorithm>

static const uint minNumberOfMaintenances = 2;

namespace Antares::Solver::TSGenerator
{
// optimization problem - methods
void OptimizedThermalGenerator::GenerateOptimizedThermalTimeSeries()
{
    setMaintenanceGroupParameters();
    if (!checkMaintenanceGroupParameters())
        return;

    // loop through all scenarios
    for (std::size_t scenarioIndex = 0; scenarioIndex < scenarioNumber_; ++scenarioIndex)
    {
        OptProblemSettings optSett;
        optSett.optProblemStartTime = 0;
        optSett.optProblemEndTime = timeHorizon_;
        // loop till the end of scenario length
        while (optSett.optProblemStartTime <= scenarioLength_ * DAYS_PER_YEAR)
        {
            createOptimizationProblemPerGroup(optSett);

            // Update the time values for the next iteration
            optSett.optProblemStartTime += timeStep_;
            optSett.optProblemEndTime += timeStep_;
        }
    }
}

void OptimizedThermalGenerator::createOptimizationProblemPerGroup(const OptProblemSettings& optSett)
{
    runOptimizationProblem();
    // ...
    // do the optimization post-processing here &
    // do the writing off the result here

    // just playing here - will ue this loops later for opt problem formulation
    // loop through the elements of weightMap weights_
    for (const auto& entryWeightMap : maintenanceGroup_)
    {
        const auto& area = *(entryWeightMap.first);
        // loop through the thermal clusters inside the area
        for (auto it = area.thermal.list.mapping.begin(); it != area.thermal.list.mapping.end();
             ++it)
        {
            auto& cluster = *(it->second);
            createOptimizationProblemPerCluster(area, cluster);
            ++pProgression;
        }
    }

    indexes.day.push_back(OptimizationProblemVariableIndexesPerDay());
    indexes.day[0].Ens = 1;
    indexes.day[0].Spill = 2;
    indexes.day[0].area.push_back(OptimizationProblemVariableIndexesPerArea());
    indexes.day[0].area[0].cluster.push_back(OptimizationProblemVariableIndexesPerCluster());
    indexes.day[0].area[0].cluster[0].unit.push_back(OptimizationProblemVariableIndexesPerUnit());
    indexes.day[0].area[0].cluster[0].unit[0].P = 100;
    indexes.day[0].area[0].cluster[0].unit[0].start.push_back(55);
    indexes.day[0].area[0].cluster[0].unit[0].end.push_back(155);
    // end play
}

void OptimizedThermalGenerator::createOptimizationProblemPerCluster(const Data::Area& area,
                                                                    Data::ThermalCluster& cluster)
{
    if (cluster.doWeGenerateTS(globalThermalTSgeneration_) && cluster.optimizeMaintenance)
    {
        // number of maintenances are defined per cluster-unit
        // but are the same for all units inside the same cluster
        int numberOfMaintenancesPerUnit;
        int averageMaintenanceDuration;
        numberOfMaintenancesPerUnit = calculateNumberOfMaintenances(
          cluster, timeHorizon_);
        averageMaintenanceDuration
          = calculateAverageMaintenanceDuration(cluster); // this will floor the double value !!
        std::array<double, DAYS_PER_YEAR> maxUnitOutput = calculateMaxUnitOutput(cluster);
        // uint earliestStartOfFirstMaintenance
        //   = calculateUnitEarliestStartOfFirstMaintenance(cluster, 8);
        // uint latestStartOfFirstMaintenance
        //   = calculateUnitLatestStartOfFirstMaintenance(cluster, 8);
        // just playing here - this needs to go into new method - class  - operator
        logs.info() << "CR27-INFO: This cluster is active for mnt planning: "
                    << cluster.getFullName();

        if (!cluster.prepro)
        {
            logs.error() << "Cluster: " << area.name << '/' << cluster.name()
                         << ": The timeseries will not be regenerated. All data "
                            "related to the ts-generator for "
                         << "'thermal' have been released.";
        }

        assert(cluster.prepro);

        if (0 == cluster.unitCount || 0 == cluster.nominalCapacity || nbThermalTimeseries == 0)
        {
            cluster.series.timeSeries.reset(1, 8760);
        }
        else
        {
            cluster.series.timeSeries.reset(nbThermalTimeseries, 8760);
            cluster.series.timeSeries.fill(777.);
        }

        if (archive)
            writeResultsToDisk(area, cluster);

        // end playing
    }
}

// calculate parameters methods - per maintenance group
void OptimizedThermalGenerator::setMaintenanceGroupParameters()
{
    // timeHorizon, timeStep ENS and Spillage are defined per one MaintenanceGroup
    // Residual Load (or reference value) array is defined per one MaintenanceGroup
    calculateResidualLoad();
    residualLoadDailyValues_ = calculateDailySums(maintenanceGroup_.getUsedResidualLoadTS());
    std::tie(ensCost_, spillCost_) = calculateMaintenanceGroupENSandSpillageCost();
    std::tie(timeStep_, timeHorizon_) = calculateTimeHorizonAndStep();
}

bool OptimizedThermalGenerator::checkMaintenanceGroupParameters()
{
    if (timeStep_ == 0)
    {
        logs.info() << "Maintenance group: " << maintenanceGroup_.name()
                    << ": The timeseries generation will be skiped:  timeStep = 0. It is possible "
                       "that the maintenance group has no clusters designated for maintenance "
                       "planning or, at least one cluster has interPoPeriod = 0";
        return false;
    }
    if (timeHorizon_ == 0)
    {
        logs.info()
          << "Maintenance group: " << maintenanceGroup_.name()
          << ": The timeseries generation will be skiped:  timeHorizon = 0. It is possible "
             "that the maintenance group has no clusters designated for maintenance "
             "planning or, at all cluster have interPoPeriod = 0";
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
            if (cluster.doWeGenerateTS(globalThermalTSgeneration_) && cluster.optimizeMaintenance)
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

// calculate parameters methods - per cluster
uint OptimizedThermalGenerator::calculateNumberOfMaintenances(Data::ThermalCluster& cluster,
                                                              uint timeHorizon)
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

    return timeHorizon / cluster.interPoPeriod; // floor
}

uint OptimizedThermalGenerator::calculateAverageMaintenanceDuration(Data::ThermalCluster& cluster)
{
    double sum = 0.0;
    for (std::size_t row = 0; row < DAYS_PER_YEAR; ++row)
    {
        sum += cluster.prepro->data[Data::PreproThermal::poDuration][row];
    }

    return sum / static_cast<double>(DAYS_PER_YEAR);
}

std::array<double, DAYS_PER_YEAR> OptimizedThermalGenerator::calculateMaxUnitOutput(
  Data::ThermalCluster& cluster)
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

// calculate parameters methods - per cluster-Unit
int OptimizedThermalGenerator::calculateUnitEarliestStartOfFirstMaintenance(
  Data::ThermalCluster& cluster,
  uint unitIndex)
{
    // earliest start of the first maintenance of unit u (beginning of the window, can be negative):
    // let it return negative value - if it returns negative value we wont implement constraint:
    // s[tauLower-1][u][1] = 0
    // TODO CR27: if the approach above breaks the solver
    // we force the start of the first maintenance to be after day=0
    // s[fixed = 0][u][1] = 0
    if (unitIndex < cluster.daysSinceLastMaintenance.size())
    {
        return (cluster.interPoPeriod - cluster.daysSinceLastMaintenance[unitIndex]
                - cluster.poWindows);
    }
    else
    {
        logs.error() << "Cluster: " << cluster.getFullName()
                     << " does not have unit: " << unitIndex;
        return 0;
    }
}

int OptimizedThermalGenerator::calculateUnitLatestStartOfFirstMaintenance(
  Data::ThermalCluster& cluster,
  uint unitIndex)
{
    // latest start of the first maintenance of unit u (end of the window, must be positive):
    if (unitIndex < cluster.daysSinceLastMaintenance.size())
    {
        // this cannot be negative:
        // cluster.interPoPeriod - cluster.daysSinceLastMaintenance[unitIndex] - is always positive
        // or zero
        // cluster.poWindows is positive or zero
        // so std::max is not necessary but I will keep it for now
        return std::max(
          cluster.interPoPeriod - cluster.daysSinceLastMaintenance[unitIndex] + cluster.poWindows,
          0);
    }
    else
    {
        logs.error() << "Cluster: " << cluster.getFullName()
                     << " does not have unit: " << unitIndex;
        return 0;
    }
}

// auxillary functions
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

bool OptimizedThermalGenerator::checkClusterData(Data::ThermalCluster& cluster)
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
        cluster.series.timeSeries.reset(1, 8760);
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
