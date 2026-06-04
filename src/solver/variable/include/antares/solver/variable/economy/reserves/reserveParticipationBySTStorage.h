// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0
#pragma once

#include "reserveParticipationTemplate.h"
#include "vCardReserveParticipationBySTStorage.h"

namespace Antares::Solver::Variable::Economy::Reserves
{

/*!
** \brief Reserve Participation from short-term storage units
*/
class ReserveParticipationBySTStorage
    : public ReserveParticipationTemplate<ReserveParticipationBySTStorage,
                                          VCardReserveParticipationBySTStorage>
{
public:
    using VCardType = VCardReserveParticipationBySTStorage;
    using AncestorType = ReserveParticipationTemplate<ReserveParticipationBySTStorage, VCardType>;

    using AncestorType::pSize;
    using AncestorType::pValuesForTheCurrentYear;

    ReserveParticipationBySTStorage() = default;

    size_t getSizeFromArea(Data::Study* /*study*/, Data::Area* area)
    {
        return area->shortTermStorage.reserveParticipationsCount();
    }

    void populateHourlyValues(State& state, unsigned int numSpace);

    bool hasIndexMapping(const Data::Study& study, const Data::Area* area) const
    {
        return study.parameters.include.reserves
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
        auto [reserveID, clusterName] = results.data.study.runtime.reserveParticipationIndexMaps
                                          .value()
                                          .at(results.data.area->id)
                                          .STStorageClusters.right.at(i);
        auto reserveName = results.data.study.runtime.reserveIDToName.value().at(reserveID);
        results.variableCaption = reserveName + "_" + clusterName;
        results.variableUnit = VCardType::Unit();
        pValuesForTheCurrentYear[numSpace][i]
          .template buildAnnualSurveyReport<VCardType>(results, fileLevel, precision);
    }

}; // class ReserveParticipationBySTStorage

inline void ReserveParticipationBySTStorage::populateHourlyValues(State& state,
                                                                  unsigned int numSpace)
{
    if (hasIndexMapping(state.study, state.area))
    {
        const auto& resData = state.reserveData.value().at(state.area->index);
        for (const auto& clusterName:
             resData.reserveParticipationPerSTStorageClusterForYear.at(state.hourInTheYear)
               | std::views::keys)
        {
            for (const auto& [reserveName, reserveParticipation]:
                 resData.reserveParticipationPerSTStorageClusterForYear.at(state.hourInTheYear)
                   .at(clusterName))
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
