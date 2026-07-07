// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include "reserveParticipationTemplate.h"
#include "vCardReserveParticipationByHydro.h"

namespace Antares::Solver::Variable::Economy::Reserves
{

/*!
** \brief Reserve Participation from hydro
*/
class ReserveParticipationByHydro
    : public ReserveParticipationTemplate<ReserveParticipationByHydro,
                                          VCardReserveParticipationByHydro>
{
public:
    using VCardType = VCardReserveParticipationByHydro;
    using AncestorType = ReserveParticipationTemplate<ReserveParticipationByHydro, VCardType>;

    using AncestorType::pSize;
    using AncestorType::pValuesForTheCurrentYear;

    ReserveParticipationByHydro() = default;

    size_t getSizeFromArea(Data::Study* /*study*/, Data::Area* area)
    {
        return area->hydro.reserveParticipationsCount();
    }

    void populateHourlyValues(State& state, unsigned int numSpace);

    bool hasIndexMapping(const Data::Study& study, const Data::Area* area) const
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

inline void ReserveParticipationByHydro::populateHourlyValues(State& state, unsigned int numSpace)
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
