// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include "DispatchablePlantByCluster_base.h"

namespace Antares::Solver::Variable::Economy
{
struct NbOfDispatchedUnitsByPlantTraits
{
    static std::string Caption()
    {
        return "NODU by plant";
    }

    static std::string Unit()
    {
        return "NODU";
    }

    static std::string Description()
    {
        return "Number of Dispatchable Units by plant";
    }

    static void setHourlyValuesForCurrentYear(std::vector<IntermediateValues>& clusterValues,
                                              State& state)
    {
        auto area = state.area;
        auto& thermal = state.thermal;
        for (auto& cluster: area->thermal.list.each_enabled())
        {
            clusterValues[cluster->enabledIndex].hour[state.hourInTheYear]
              = thermal[area->index].numberOfUnitsONbyCluster[cluster->enabledIndex];
        }
    }

    static void yearEndBuildPrepareDataForEachThermalCluster(
      std::vector<std::vector<IntermediateValues>>& yearlyValues,
      State& state,
      uint,
      unsigned int numSpace)
    {
        for (unsigned int i = 0; i <= state.study.runtime.rangeLimits.hour[Data::rangeEnd]; ++i)
        {
            state.thermalClusterDispatchedUnitsCountForYear[i] += static_cast<uint>(
              yearlyValues[numSpace][state.thermalCluster->enabledIndex].hour[i]);
        }
    }

    static void yearEndBuildForEachThermalCluster(
      std::vector<std::vector<IntermediateValues>>& yearlyValues,
      State& state,
      uint,
      unsigned int numSpace)
    {
        for (unsigned int i = state.study.runtime.rangeLimits.hour[Data::rangeBegin];
             i <= state.study.runtime.rangeLimits.hour[Data::rangeEnd];
             ++i)
        {
            yearlyValues[numSpace][state.thermalCluster->enabledIndex].hour[i]
              = state.thermalClusterDispatchedUnitsCountForYear[i];
        }
    }

    static Antares::Memory::Stored<double>::ConstReturnType retrieveRawHourlyValuesForCurrentYear(
      const std::vector<std::vector<IntermediateValues>>& yearlyValues,
      unsigned int,
      unsigned int numSpace)
    {
        return yearlyValues[numSpace][0].hour;
    }
};

using VCardNbOfDispatchedUnitsByPlant = VCardDispatchablePlantByClusterBase<
  NbOfDispatchedUnitsByPlantTraits>;

/*!
** \brief C02 Average value of the overrall OperatingCost emissions expected from all
**   the thermal dispatchable clusters
*/
template<class NextT = Container::EndOfList>
using NbOfDispatchedUnitsByPlant = DispatchablePlantByClusterBase<NbOfDispatchedUnitsByPlantTraits,
                                                                  NextT>;

} // namespace Antares::Solver::Variable::Economy
