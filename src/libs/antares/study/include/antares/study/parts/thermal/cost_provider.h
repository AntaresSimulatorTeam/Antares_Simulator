// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

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

class ConstantCostProvider final: public CostProvider
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

class ScenarizedCostProvider final: public CostProvider
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
