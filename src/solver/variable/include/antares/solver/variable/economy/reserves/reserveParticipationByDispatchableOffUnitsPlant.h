// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0
#pragma once

#include "reserveParticipationTemplate.h"
#include "vCardReserveParticipationByDispatchableOffUnitsPlant.h"

namespace Antares::Solver::Variable::Economy::Reserves
{

/*!
** \brief Reserve Participation from off units in thermal clusters
*/
class ReserveParticipationByDispatchableOffUnitsPlant
    : public ReserveParticipationTemplate<ReserveParticipationByDispatchableOffUnitsPlant,
                                          VCardReserveParticipationByDispatchableOffUnitsPlant>
{
public:
    using VCardType = VCardReserveParticipationByDispatchableOffUnitsPlant;
    using AncestorType = ReserveParticipationTemplate<
      ReserveParticipationByDispatchableOffUnitsPlant,
      VCardType>;

    using AncestorType::pNbYearsParallel;
    using AncestorType::pSize;
    using AncestorType::pValuesForTheCurrentYear;

    ReserveParticipationByDispatchableOffUnitsPlant() = default;

    size_t getSizeFromArea(Data::Study* /*study*/, Data::Area* area)
    {
        return area->thermal.list.reserveParticipationsCount();
    }

    void populateHourlyValues(State& state, unsigned int numSpace);

    bool hasIndexMapping(const Data::Study& study, const Data::Area* area) const
    {
        return study.parameters.include.reserves
               && !study.runtime.reserveParticipationIndexMaps.value()
                     .at(area->id)
                     .thermalClusters.empty();
    }

    void buildReportForIndex(SurveyResults& results,
                             uint i,
                             int fileLevel,
                             int precision,
                             unsigned int numSpace) const
    {
        auto [reserveID, clusterName] = results.data.study.runtime.reserveParticipationIndexMaps
                                          .value()
                                          .at(results.data.area->id)
                                          .thermalClusters.right.at(i);
        auto reserveName = results.data.study.runtime.reserveIDToName.value().at(reserveID);
        results.variableCaption = reserveName + "_" + clusterName + "_off";
        results.variableUnit = VCardType::Unit();
        pValuesForTheCurrentYear[numSpace][i]
          .template buildAnnualSurveyReport<VCardType>(results, fileLevel, precision);
    }

}; // class ReserveParticipationByDispatchableOffUnitsPlant

inline void ReserveParticipationByDispatchableOffUnitsPlant::populateHourlyValues(
  State& state,
  unsigned int numSpace)
{
    if (hasIndexMapping(state.study, state.area))
    {
        const auto& resData = state.reserveData.value().at(state.area->index);
        for (const auto& clusterName:
             resData.reserveParticipationPerThermalClusterForYear.at(state.hourInTheYear)
               | std::views::keys)
        {
            for (const auto& [reserveName, reserveParticipation]:
                 resData.reserveParticipationPerThermalClusterForYear.at(state.hourInTheYear)
                   .at(clusterName))
            {
                pValuesForTheCurrentYear[numSpace]
                                        [state.study.runtime.reserveParticipationIndexMaps.value()
                                           .at(state.area->id)
                                           .thermalClusters.left.at(
                                             std::make_pair(reserveName, clusterName))]
                                          .hour[state.hourInTheYear]
                  = reserveParticipation.offUnitsParticipation;
            }
        }
    }
}

} // namespace Antares::Solver::Variable::Economy::Reserves
