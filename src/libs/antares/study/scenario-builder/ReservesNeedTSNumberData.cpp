// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

//
// Created by marechaljas on 04/07/23.
//

#include "antares/study/scenario-builder/ReservesNeedTSNumberData.h"

#include "antares/study/scenario-builder/applyToMatrix.hxx"

namespace Antares::Data::ScenarioBuilder
{
bool ReservesNeedTSNumberData::reset(const Study& study)
{
    const uint nbYears = study.parameters.nbYears;
    assert(pArea != nullptr);

    uint reservesCount = pArea->allCapacityReservations.value().areaCapacityReservations.size();

    // Resize
    pTSNumberRules.reset(reservesCount, nbYears);
    return true;
}

void ReservesNeedTSNumberData::setTSnumber(const Antares::Data::CapacityReservation& res,
                                           const uint year,
                                           uint value)
{
    if (year < pTSNumberRules.height && res.areaWideIndex < pTSNumberRules.width)
    {
        pTSNumberRules[res.areaWideIndex][year] = value;
    }
}

bool ReservesNeedTSNumberData::apply(Study& study)
{
    bool ret = true;
    CString<512, false> logprefix;
    // Errors
    uint errors = 0;

    // The total number of areas;
    const uint areaCount = study.areas.size();

    for (uint areaIndex = 0; areaIndex != areaCount; ++areaIndex)
    {
        // Alias to the current area
        Area& area = *(study.areas.byIndex[areaIndex]);
        // alias to the current column
        assert(areaIndex < pTSNumberRules.width);
        const MatrixType::ColumnType& col = pTSNumberRules[areaIndex];

        logprefix.clear() << "Reserves Need: Area '" << area.name << "': ";
        for (const auto& [reserveID, reserve]:
             area.allCapacityReservations.value().areaCapacityReservations)
        {
            logprefix.clear() << "Reserves Need: Area '" << area.name << "', Reserve '" << reserveID
                              << "': ";
            ret = ApplyToMatrix(errors, logprefix, *reserve.need, col, 0) && ret;
        }
    }
    return ret;
}
} // namespace Antares::Data::ScenarioBuilder
