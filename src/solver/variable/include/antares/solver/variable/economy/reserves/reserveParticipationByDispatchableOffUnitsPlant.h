// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0
#pragma once

#include "reserveParticipationTemplate.h"

namespace Antares::Solver::Variable::Economy::Reserves
{

/*!
** \brief Reserve Participation from off units in thermal clusters
*/
template<class NextT = Container::EndOfList>
class ReserveParticipationByDispatchableOffUnitsPlant
    : public ReserveParticipationTemplate<ReserveParticipationByDispatchableOffUnitsPlant<NextT>,
                                          VCardReserveParticipationByDispatchableOffUnitsPlant,
                                          NextT>
{
public:
    //! Type of the next static variable
    typedef NextT NextType;
    //! VCard
    typedef VCardReserveParticipationByDispatchableOffUnitsPlant VCardType;
    //! Ancestor
    typedef ReserveParticipationTemplate<ReserveParticipationByDispatchableOffUnitsPlant<NextT>,
                                         VCardType,
                                         NextT>
      AncestorType;

    using AncestorType::pNbYearsParallel;
    using AncestorType::pSize;
    using AncestorType::pValuesForTheCurrentYear;

    ReserveParticipationByDispatchableOffUnitsPlant() = default;

    size_t getSizeFromArea(Study* /*study*/, Area* area)
    {
        return area->thermal.list.reserveParticipationsCount();
    }

    void populateHourlyValues(State& state, unsigned int numSpace);

    bool hasIndexMapping(const Study& study, const Area* area) const
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

template<class NextT>
void ReserveParticipationByDispatchableOffUnitsPlant<NextT>::populateHourlyValues(
  /*non const*/ State& state,
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
