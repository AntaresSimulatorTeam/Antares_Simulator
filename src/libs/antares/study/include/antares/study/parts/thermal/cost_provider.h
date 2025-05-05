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

#pragma once

#include <array>
#include <vector>

namespace Antares::Data
{
class CostProvider
{
public:
    virtual ~CostProvider() = default;
    virtual double getOperatingCost(uint serieIndex, uint hourInTheYear) const = 0;
    virtual double getMarginalCost(uint serieIndex, uint hourInTheYear) const = 0;
    virtual double getMarketBidCost(uint hourInTheYear, uint year) const = 0;
};

class ThermalCluster;

class ConstantCostProvider: public CostProvider
{
public:
    explicit ConstantCostProvider(const ThermalCluster& cluster);
    virtual ~ConstantCostProvider() = default;
    double getOperatingCost(uint serieIndex, uint hourInTheYear) const override;
    double getMarginalCost(uint serieIndex, uint hourInTheYear) const override;
    double getMarketBidCost(uint hourInTheYear, uint year) const override;

private:
    const ThermalCluster& cluster;
};

class ScenarizedCostProvider: public CostProvider
{
public:
    explicit ScenarizedCostProvider(const ThermalCluster& cluster);
    virtual ~ScenarizedCostProvider() = default;
    double getOperatingCost(uint serieIndex, uint hourInTheYear) const override;
    double getMarginalCost(uint serieIndex, uint hourInTheYear) const override;
    double getMarketBidCost(uint hourInTheYear, uint year) const override;

private:
    /*!
    ** \brief Production Cost, Market Bid Cost and Marginal Cost Matrixes - Per Hour and per Time
    *Series
    */
    struct CostsTimeSeries
    {
        std::array<double, HOURS_PER_YEAR> productionCostTs;
        std::array<double, HOURS_PER_YEAR> marketBidCostTS;
        std::array<double, HOURS_PER_YEAR> marginalCostTS;
    };

    void resizeCostTS();
    void ComputeMarketBidTS();
    void MarginalCostEqualsMarketBid();
    void ComputeProductionCostTS();

    std::vector<CostsTimeSeries> costsTimeSeries;
    const ThermalCluster& cluster;
};
} // namespace Antares::Data
