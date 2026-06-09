// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include "dynamic_multi_column_base.h"
#include "economy_base.h"

namespace Antares::Solver::Variable::Economy
{

struct DispatchableGenerationTraits
{
    static std::string Caption()
    {
        return "Dispatch. Gen.";
    }

    static std::string Unit()
    {
        return "MWh";
    }

    static std::string Description()
    {
        return "Value of all the dispatchable generation throughout all MC years";
    }

    using ResultsProfile = StandardResults<>;

    static constexpr uint8_t decimal = 0;

    static std::vector<ColumnDescriptor> buildColumnDescriptors(Data::Area* area)
    {
        std::vector<ColumnDescriptor> descriptors;
        std::map<std::string, size_t> groupNumbers;

        for (auto& cluster: area->thermal.list.each_enabled())
        {
            const std::string& group = cluster->getGroup();
            if (groupNumbers.find(group) == groupNumbers.end())
            {
                size_t idx = descriptors.size();
                groupNumbers[group] = idx;
                descriptors.push_back({group, "MWh"});
            }
        }
        return descriptors;
    }

    static void setHourlyValue(
      VCardDynamicMultiColumn<DispatchableGenerationTraits>::IntermediateValuesBaseType& pValues,
      State& state,
      unsigned int,
      const std::vector<ColumnDescriptor>& descriptors)
    {
        auto& area = state.area;
        auto& thermal = state.thermal;
        std::map<std::string, size_t> groupToNumbers;
        for (size_t i = 0; i < descriptors.size(); ++i)
        {
            groupToNumbers[descriptors[i].caption] = i;
        }
        for (auto& cluster: area->thermal.list.each_enabled())
        {
            size_t groupNumber = groupToNumbers.at(cluster->getGroup());
            pValues[groupNumber][state.hourInTheYear] += thermal[area->index]
                                                           .thermalClustersProductions
                                                             [cluster->enabledIndex];
        }
    }
};

using DispatchableGeneration = DynamicMultiColumnBase<DispatchableGenerationTraits>;

} // namespace Antares::Solver::Variable::Economy
