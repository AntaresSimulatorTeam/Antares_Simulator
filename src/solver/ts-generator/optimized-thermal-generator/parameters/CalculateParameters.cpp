//
// Created by milos on 14/11/23.
//

#include "../main/OptimizedGenerator.h"

namespace Antares::Solver::TSGenerator
{

// methods are ordered in the line of execution order
void OptimizationParameters::allocateClusterData()
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
            clusterData[&cluster] = ClusterData();
            clusterData[&cluster].maintenanceEnabled
              = (cluster.doWeGenerateTS(globalThermalTSgeneration_) && cluster.optimizeMaintenance);
            clusterData[&cluster].staticInputs = StaticInputs();
            clusterData[&cluster].dynamicInputs = DynamicInputs();
            clusterData[&cluster].dynamicResults = DynamicResults();
        }
    }
}

void OptimizationParameters::calculateNonDependantClusterData()
{
    for (auto& clusterEntry : clusterData)
    {
        auto& data = clusterEntry.second;
        const auto& cluster = *(clusterEntry.first);

        // static Inputs
        // this inputs wont be changed during optimization problem
        // and are important to calculate immediately
        // cause their values are used for calculating other parameters
        data.staticInputs.maxPower = calculateMaxUnitOutput(cluster);
        data.staticInputs.avgCost = calculateAvrUnitDailyCost(cluster);

        if (!data.maintenanceEnabled)
            continue;

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

void OptimizationParameters::calculateResidualLoad()
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
        auto tmpRenewable = calculateAverageRenewableTs(modelingType_, area);

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

std::pair<double, double> OptimizationParameters::calculateMaintenanceGroupENSandSpillageCost()
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

int OptimizationParameters::calculateTimeStep()
{
    std::vector<int> averageDurationBetweenMaintenances = {};
    for (const auto& clusterEntry : clusterData)
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

int OptimizationParameters::calculateTimeHorizon()
{
    std::vector<int> timeHorizonVector = {};
    for (const auto& clusterEntry : clusterData)
    {
        const auto& cluster = *(clusterEntry.first);
        if (!(cluster.doWeGenerateTS(globalThermalTSgeneration_) && cluster.optimizeMaintenance))
            continue;
        for (int unit = 0; unit < cluster.unitCount; ++unit)
        {
            int value = 2 * getAverageDurationBetweenMaintenances(cluster)
                        + getAverageMaintenanceDuration(cluster)
                        - std::min(getAverageDurationBetweenMaintenances(cluster),
                                   getDaysSinceLastMaintenance(cluster, unit))
                        + 1;
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

void OptimizationParameters::calculateDependantClusterData()
{
    for (auto& clusterEntry : clusterData)
    {
        auto& data = clusterEntry.second;
        const auto& cluster = *(clusterEntry.first);

        if (!data.maintenanceEnabled)
            continue;
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

void OptimizationParameters::setMaintenanceGroupParameters()
{
    // it is crucial that we allocateClusterData
    // and calculate NonDependantClusterData
    // since later methods expect this to be filled
    // and values available
    allocateClusterData();
    calculateNonDependantClusterData();
    //
    calculateResidualLoad();
    residualLoadDailyValues_ = calculateDailySums(maintenanceGroup_.getUsedResidualLoadTS());
    std::tie(ensCost_, spillCost_) = calculateMaintenanceGroupENSandSpillageCost();
    timeStep_ = calculateTimeStep();
    timeHorizon_ = calculateTimeHorizon();
    timeHorizonFirstStep_ = timeHorizon_;
    // calculateDependantClusterData
    // uses timeHorizon_ so it is important we calculate timeHorizon_ first
    calculateDependantClusterData();
}

bool OptimizationParameters::checkMaintenanceGroupParameters()
{
    if (timeStep_ <= 0)
    {
        logs.warning() << "Maintenance group: " << maintenanceGroup_.name()
                    << ": The timeseries generation will be skiped:  timeStep = 0. It is possible "
                       "that the maintenance group has no clusters designated for maintenance "
                       "planning, or at least one cluster has interPoPeriod = 0";
        return false;
    }
    if (timeHorizon_ <= 0)
    {
        logs.warning() << "Maintenance group: " << maintenanceGroup_.name()
                    << ": The timeseries generation will be skiped:  timeHorizon <= 0";
        return false;
    }
    // add some more check here if necessary!
    return true;
}

bool OptimizationParameters::checkTimeHorizon(OptProblemSettings& optSett)
{
    if (timeHorizon_ <= 0)
    {
        logs.warning() << "Maintenance group: " << maintenanceGroup_.name()
                       << ". Scenario Num: " << optSett.scenario
                       << ". Optimization stopped in step: " << optSett.firstDay << ".Day - "
                       << optSett.lastDay
                       << ".Day. TimeHorizon <= 0. This scenario wont have generated timeseries";
        optSett.solved = false;
        return false;
    }

    return true;
}

} // namespace Antares::Solver::TSGenerator
