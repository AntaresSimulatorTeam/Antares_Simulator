//
// Created by milos on 14/11/23.
//

#include "../Main/OptimizedGenerator.h"

namespace Antares::Solver::TSGenerator
{
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

} // namespace Antares::Solver::TSGenerator
