/*
 * Copyright 2007-2024, RTE (https://www.rte-france.com)
 * See AUTHORS.txt
 * SPDX-License-Identifier: MPL-2.0
 * This file is part of Antares-Simulator,
 * Adequacy and Performance assessment for interconnected energy networks.
 *
 * Antares_Simulator is free software: you can redistribute it and/or modify
 * it under the terms of the Mozilla Public Licence 2.0 as published by
 * the Mozilla Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * Antares_Simulator is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * Mozilla Public Licence 2.0 for more details.
 *
 * You should have received a copy of the Mozilla Public Licence 2.0
 * along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
 */

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

        for (uint hour = 0; hour < HOURS_PER_YEAR; ++hour)
        {
            double hourlyModulation = cluster.modulation[Data::thermalModulationCost][hour];
            productionCostTS[hour] = marginalCostTS[hour] * hourlyModulation;
        }
    }
}

void ScenarizedCostProvider::resizeCostTS()
{
    const uint fuelCostWidth = cluster.ecoInput.fuelcost.width;
    const uint co2CostWidth = cluster.ecoInput.co2cost.width;
    const uint tsCount = std::max(fuelCostWidth, co2CostWidth);

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
    const uint fuelCostWidth = cluster.ecoInput.fuelcost.width;
    const uint co2CostWidth = cluster.ecoInput.co2cost.width;

    double co2EmissionFactor = cluster.emissions.factors[Pollutant::CO2];

    for (uint tsIndex = 0; tsIndex < costsTimeSeries.size(); ++tsIndex)
    {
        uint tsIndexFuel = std::min(fuelCostWidth - 1, tsIndex);
        uint tsIndexCo2 = std::min(co2CostWidth - 1, tsIndex);
        for (uint hour = 0; hour < HOURS_PER_YEAR; ++hour)
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

double ScenarizedCostProvider::getOperatingCost(uint serieIndex, uint hourInTheYear) const
{
    const uint tsIndex = std::min(serieIndex, static_cast<uint>(costsTimeSeries.size()) - 1);
    return costsTimeSeries[tsIndex].productionCostTs[hourInTheYear];
}

double ScenarizedCostProvider::getMarginalCost(uint serieIndex, uint hourInTheYear) const
{
    const double mod = cluster.modulation[thermalModulationMarketBid][hourInTheYear];
    const uint tsIndex = std::min(serieIndex, static_cast<uint>(costsTimeSeries.size()) - 1);
    return costsTimeSeries[tsIndex].marginalCostTS[hourInTheYear] * mod;
}

double ScenarizedCostProvider::getMarketBidCost(uint hourInTheYear, uint year) const
{
    const double mod = cluster.modulation[thermalModulationMarketBid][hourInTheYear];
    const uint serieIndex = cluster.series.getSeriesIndex(year);
    const uint tsIndex = std::min(serieIndex, static_cast<uint>(costsTimeSeries.size()) - 1);
    return costsTimeSeries[tsIndex].marketBidCostTS[hourInTheYear] * mod;
}

} // namespace Antares::Data
