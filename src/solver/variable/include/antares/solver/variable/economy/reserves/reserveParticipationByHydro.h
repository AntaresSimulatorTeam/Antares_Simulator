// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include "reserveParticipationTemplate.h"

namespace Antares::Solver::Variable::Economy::Reserves
{

/*!
** \brief Reserve Participation from hydro
*/
template<class NextT = Container::EndOfList>
class ReserveParticipationByHydro
    : public ReserveParticipationTemplate<ReserveParticipationByHydro<NextT>,
                                          VCardReserveParticipationByHydro,
                                          NextT>
{
public:
    //! Type of the next static variable
    typedef NextT NextType;
    //! VCard
    typedef VCardReserveParticipationByHydro VCardType;
    //! Ancestor
    typedef ReserveParticipationTemplate<ReserveParticipationByHydro<NextT>, VCardType, NextT>
      AncestorType;

    using AncestorType::pSize;
    using AncestorType::pValuesForTheCurrentYear;

    ReserveParticipationByHydro() = default;

    size_t getSizeFromArea(Study* /*study*/, Area* area)
    {
        return area->hydro.reserveParticipationsCount();
    }

    void populateHourlyValues(/*non const*/ State& state, unsigned int numSpace);

    bool hasIndexMapping(const Study& study, const Area* area) const
    {
        return study.parameters.include.reserves
               && !study.runtime.reserveParticipationIndexMaps.value().at(area->id).Hydro.empty();
    }

    void buildReportForIndex(SurveyResults& results,
                             uint i,
                             int fileLevel,
                             int precision,
                             unsigned int numSpace) const
    {
        const auto reserveID = results.data.study.runtime.reserveParticipationIndexMaps.value()
                                 .at(results.data.area->id)
                                 .Hydro.right.at(i);
        auto reserveName = results.data.study.runtime.reserveIDToName.value().at(reserveID);
        results.variableCaption = reserveName + "_Hydro";
        results.variableUnit = VCardType::Unit();
        pValuesForTheCurrentYear[numSpace][i]
          .template buildAnnualSurveyReport<VCardType>(results, fileLevel, precision);
    }

}; // class ReserveParticipationByHydro

template<class NextT>
void ReserveParticipationByHydro<NextT>::populateHourlyValues(State& state, unsigned int numSpace)
{
    if (hasIndexMapping(state.study, state.area))
    {
        for (const auto& [reserveName, reserveParticipation]:
             state.reserveData.value()
               .at(state.area->index)
               .reserveParticipationPerHydroForYear[state.hourInTheYear]["Hydro"])
        {
            pValuesForTheCurrentYear[numSpace]
                                    [state.study.runtime.reserveParticipationIndexMaps.value()
                                       .at(state.area->id)
                                       .Hydro.left.at(reserveName)]
                                      .hour[state.hourInTheYear]
              = reserveParticipation;
        }
    }
}

} // namespace Antares::Solver::Variable::Economy::Reserves
