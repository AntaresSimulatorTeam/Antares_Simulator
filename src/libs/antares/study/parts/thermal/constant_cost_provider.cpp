// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/study/parts/thermal/cluster.h"

namespace Antares::Data
{
ConstantCostProvider::ConstantCostProvider(const ThermalCluster& cluster):
    cluster(cluster)
{
}

double ConstantCostProvider::getOperatingCost(unsigned int /*serieIndex*/,
                                              unsigned int hourInTheYear) const
{
    const auto* modCost = cluster.modulation[thermalModulationCost];
    return cluster.marginalCost * modCost[hourInTheYear];
}

double ConstantCostProvider::getMarginalCost(unsigned int /*serieIndex*/,
                                             unsigned int hourInTheYear) const
{
    const double mod = cluster.modulation[Data::thermalModulationCost][hourInTheYear];
    return cluster.marginalCost * mod;
}

double ConstantCostProvider::getMarketBidCost(unsigned int hourInTheYear,
                                              unsigned int /*year*/) const
{
    const double mod = cluster.modulation[thermalModulationMarketBid][hourInTheYear];
    return cluster.marketBidCost * mod;
}
} // namespace Antares::Data
