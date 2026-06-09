// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include "DispatchablePlantByCluster_base.h"

namespace Antares::Solver::Variable::Economy
{
struct MinDispatchableGenByPlantTraits
{
    static std::string Caption()
    {
        return "MIN DTG by plant";
    }

    static std::string Unit()
    {
        return "MIN GEN - MWh";
    }

    static std::string Description()
    {
        return "Min power by cluster";
    }

    static void setHourlyValue(std::vector<IntermediateValues>& clusterValues,
                               State& state,
                               [[maybe_unused]] unsigned int numSpace)
    {
        auto& area = state.area;
        auto& thermal = state.thermal;
        for (auto& cluster: area->thermal.list.each_enabled())
        {
            double minGen = cluster->PthetaInf[state.hourInTheYear];
            double production = thermal[area->index]
                                  .thermalClustersProductions[cluster->enabledIndex];

            clusterValues[cluster->enabledIndex].hour[state.hourInTheYear] += std::min(production,
                                                                                       minGen);
        }
    }
};

using MinDispatchableGenByPlant = DispatchablePlantByClusterBase<MinDispatchableGenByPlantTraits>;

} // namespace Antares::Solver::Variable::Economy
