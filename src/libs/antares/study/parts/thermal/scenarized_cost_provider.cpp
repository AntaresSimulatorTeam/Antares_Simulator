// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/study/parts/thermal/cluster.h"

namespace Antares::Data
{

ScenarizedCostProvider::ScenarizedCostProvider(const ThermalCluster& cluster):
    cluster(cluster)
{
    resizeCostTS();
    ComputeMarketBidTS();
    MarginalCostEqualsMarketBid();
    ComputeProductionCostTS();
}

void ScenarizedCostProvider::ComputeProductionCostTS()
{
    if (cluster.modulation.width == 0)
    {
        return;
    }

    for (auto& timeSeries: costsTimeSeries)
    {
        auto& productionCostTS = timeSeries.productionCostTs;
        auto& marginalCostTS = timeSeries.marginalCostTS;

        for (unsigned int hour = 0; hour < HOURS_PER_YEAR; ++hour)
        {
            double hourlyModulation = cluster.modulation[Data::thermalModulationCost][hour];
            productionCostTS[hour] = marginalCostTS[hour] * hourlyModulation;
        }
    }
}

void ScenarizedCostProvider::resizeCostTS()
{
    const unsigned int fuelCostWidth = cluster.ecoInput.fuelcost.width;
    const unsigned int co2CostWidth = cluster.ecoInput.co2cost.width;
    const unsigned int tsCount = std::max(fuelCostWidth, co2CostWidth);

    costsTimeSeries.resize(tsCount, CostsTimeSeries());
}

void ScenarizedCostProvider::MarginalCostEqualsMarketBid()
{
    for (auto& timeSeries: costsTimeSeries)
    {
        auto& source = timeSeries.marketBidCostTS;
        auto& destination = timeSeries.marginalCostTS;
        std::copy(source.begin(), source.end(), destination.begin());
    }
}

double computeMarketBidCost(double fuelCost,
                            double fuelEfficiency,
                            double co2EmissionFactor,
                            double co2cost,
                            double variableomcost)
{
    return fuelCost * 360.0 / fuelEfficiency + co2EmissionFactor * co2cost + variableomcost;
}

void ScenarizedCostProvider::ComputeMarketBidTS()
{
    const unsigned int fuelCostWidth = cluster.ecoInput.fuelcost.width;
    const unsigned int co2CostWidth = cluster.ecoInput.co2cost.width;

    double co2EmissionFactor = cluster.emissions.factors[Pollutant::CO2];

    for (unsigned int tsIndex = 0; tsIndex < costsTimeSeries.size(); ++tsIndex)
    {
        unsigned int tsIndexFuel = std::min(fuelCostWidth - 1, tsIndex);
        unsigned int tsIndexCo2 = std::min(co2CostWidth - 1, tsIndex);
        for (unsigned int hour = 0; hour < HOURS_PER_YEAR; ++hour)
        {
            double fuelcost = cluster.ecoInput.fuelcost[tsIndexFuel][hour];
            double co2cost = cluster.ecoInput.co2cost[tsIndexCo2][hour];

            costsTimeSeries[tsIndex].marketBidCostTS[hour] = computeMarketBidCost(
              fuelcost,
              cluster.fuelEfficiency,
              co2EmissionFactor,
              co2cost,
              cluster.variableomcost);
        }
    }
}

double ScenarizedCostProvider::getOperatingCost(unsigned int serieIndex,
                                                unsigned int hourInTheYear) const
{
    const unsigned int tsIndex = std::min(serieIndex,
                                          static_cast<unsigned int>(costsTimeSeries.size()) - 1);
    return costsTimeSeries[tsIndex].productionCostTs[hourInTheYear];
}

double ScenarizedCostProvider::getMarginalCost(unsigned int serieIndex,
                                               unsigned int hourInTheYear) const
{
    const double mod = cluster.modulation[thermalModulationMarketBid][hourInTheYear];
    const unsigned int tsIndex = std::min(serieIndex,
                                          static_cast<unsigned int>(costsTimeSeries.size()) - 1);
    return costsTimeSeries[tsIndex].marginalCostTS[hourInTheYear] * mod;
}

double ScenarizedCostProvider::getMarketBidCost(unsigned int hourInTheYear, unsigned int year) const
{
    const double mod = cluster.modulation[thermalModulationMarketBid][hourInTheYear];
    const unsigned int serieIndex = cluster.series.getSeriesIndex(year);
    const unsigned int tsIndex = std::min(serieIndex,
                                          static_cast<unsigned int>(costsTimeSeries.size()) - 1);
    return costsTimeSeries[tsIndex].marketBidCostTS[hourInTheYear] * mod;
}

} // namespace Antares::Data
