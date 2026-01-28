// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/study/parts/thermal/cluster.h"

namespace Antares::Data
{
ConstantCostProvider::ConstantCostProvider(const ThermalCluster& cluster):
    cluster(cluster)
{
}

double ConstantCostProvider::getOperatingCost(uint /*serieIndex*/, uint hourInTheYear) const
{
    const auto* modCost = cluster.modulation[thermalModulationCost];
    return cluster.marginalCost * modCost[hourInTheYear];
}

double ConstantCostProvider::getMarginalCost(uint /*serieIndex*/, uint hourInTheYear) const
{
    const double mod = cluster.modulation[Data::thermalModulationCost][hourInTheYear];
    return cluster.marginalCost * mod;
}

double ConstantCostProvider::getMarketBidCost(uint hourInTheYear, uint /*year*/) const
{
    const double mod = cluster.modulation[thermalModulationMarketBid][hourInTheYear];
    return cluster.marketBidCost * mod;
}
} // namespace Antares::Data
