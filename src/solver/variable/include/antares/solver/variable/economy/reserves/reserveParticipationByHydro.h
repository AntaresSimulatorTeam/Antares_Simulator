/*
** Copyright 2007-2023 RTE
** Authors: Antares_Simulator Team
**
** This file is part of Antares_Simulator.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** There are special exceptions to the terms and conditions of the
** license as they are applied to this software. View the full text of
** the exceptions in file COPYING.txt in the directory of this software
** distribution
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Antares_Simulator. If not, see <http://www.gnu.org/licenses/>.
**
** SPDX-License-Identifier: licenceRef-GPL3_WITH_RTE-Exceptions
*/
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
        return study.parameters.reservesEnabled
               && !study.runtime.reserveParticipationIndexMaps.value().at(area->id).Hydro.empty();
    }

    void buildReportForIndex(SurveyResults& results,
                             uint i,
                             int fileLevel,
                             int precision,
                             unsigned int numSpace) const
    {
        const auto reserveName = results.data.study.runtime.reserveParticipationIndexMaps.value()
                                   .at(results.data.area->id)
                                   .Hydro.right.at(i);
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
             state.reserveParticipationPerHydroForYear[state.hourInTheYear]["Hydro"])
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
