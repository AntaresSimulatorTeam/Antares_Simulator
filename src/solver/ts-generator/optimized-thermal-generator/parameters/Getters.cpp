//
// Created by milos on 14/11/23.
//

#include "../main/OptimizedGenerator.h"

namespace Antares::Solver::TSGenerator
{
// Getters
double OptimizationParameters::getPowerCost(const Data::ThermalCluster& cluster,
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

    return clusterData[&cluster].staticInputs.avgCost[dayOfTheYear(optimizationDay)];
}

double OptimizationParameters::getPowerOutput(const Data::ThermalCluster& cluster,
                                              int optimizationDay)
{
    return clusterData[&cluster].staticInputs.maxPower[dayOfTheYear(optimizationDay)];
}

double OptimizationParameters::getResidualLoad(int optimizationDay)
{
    return residualLoadDailyValues_[dayOfTheYear(optimizationDay)];
}

int OptimizationParameters::getNumberOfMaintenances(const Data::ThermalCluster& cluster, int unit)
{
    return clusterData[&cluster].dynamicInputs.numberOfMaintenances[unit];
}

int OptimizationParameters::getAverageMaintenanceDuration(const Data::ThermalCluster& cluster)
{
    return clusterData[&cluster].staticInputs.averageMaintenanceDuration;
}

int OptimizationParameters::getAverageDurationBetweenMaintenances(
  const Data::ThermalCluster& cluster)
{
    return cluster.interPoPeriod;
}

int OptimizationParameters::getDaysSinceLastMaintenance(const Data::ThermalCluster& cluster,
                                                        int unit)
{
    return clusterData[&cluster].dynamicInputs.daysSinceLastMaintenance[unit];
}

} // namespace Antares::Solver::TSGenerator
