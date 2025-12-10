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
** \brief Reserve Participation from short-term storage units
*/
template<class NextT = Container::EndOfList>
class ReserveParticipationBySTStorage
    : public ReserveParticipationTemplate<ReserveParticipationBySTStorage<NextT>,
                                          VCardReserveParticipationBySTStorage,
                                          NextT>
{
public:
    //! Type of the next static variable
    typedef NextT NextType;
    //! VCard
    typedef VCardReserveParticipationBySTStorage VCardType;
    //! Ancestor
    typedef ReserveParticipationTemplate<ReserveParticipationBySTStorage<NextT>, VCardType, NextT>
      AncestorType;

    using AncestorType::pSize;
    using AncestorType::pValuesForTheCurrentYear;

    ReserveParticipationBySTStorage() = default;

    size_t getSizeFromArea(Study*, Area* area)
    {
        return area->shortTermStorage.reserveParticipationsCount();
    }

    void populateHourlyValues(/*non const*/ State& state, unsigned int numSpace);

    bool hasIndexMapping(const Study& study, const Area* area) const
    {
        return study.parameters.reservesEnabled
               && !study.runtime.reserveParticipationIndexMaps.value()
                     .at(area->id)
                     .STStorageClusters.empty();
    }

    void buildReportForIndex(SurveyResults& results,
                             uint i,
                             int fileLevel,
                             int precision,
                             unsigned int numSpace) const
    {
        auto [reserveName, clusterName] = results.data.study.runtime.reserveParticipationIndexMaps
                                            .value()
                                            .at(results.data.area->id)
                                            .STStorageClusters.right.at(i);
        results.variableCaption = reserveName + "_" + clusterName;
        results.variableUnit = VCardType::Unit();
        pValuesForTheCurrentYear[numSpace][i]
          .template buildAnnualSurveyReport<VCardType>(results, fileLevel, precision);
    }

}; // class ReserveParticipationBySTStorage

template<class NextT>
void ReserveParticipationBySTStorage<NextT>::populateHourlyValues(State& state,
                                                                  unsigned int numSpace)
{
    if (hasIndexMapping(state.study, state.area))
    {
        for (const auto& clusterName:
             state.reserveParticipationPerSTStorageClusterForYear[state.hourInTheYear]
               | std::views::keys)
        {
            for (const auto& [reserveName, reserveParticipation]:
                 state.reserveParticipationPerSTStorageClusterForYear[state.hourInTheYear]
                                                                     [clusterName])
            {
                pValuesForTheCurrentYear[numSpace]
                                        [state.study.runtime.reserveParticipationIndexMaps.value()
                                           .at(state.area->id)
                                           .STStorageClusters.left.at(
                                             std::make_pair(reserveName, clusterName))]
                                          .hour[state.hourInTheYear]
                  = reserveParticipation;
            }
        }
    }
}

} // namespace Antares::Solver::Variable::Economy::Reserves
