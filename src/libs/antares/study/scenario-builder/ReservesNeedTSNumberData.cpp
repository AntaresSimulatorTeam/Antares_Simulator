// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/study/scenario-builder/ReservesNeedTSNumberData.h"

#include "antares/study/scenario-builder/applyToMatrix.hxx"

namespace Antares::Data::ScenarioBuilder
{
bool ReservesNeedTSNumberData::reset(const Study& study)
{
    const uint nbYears = study.parameters.nbYears;
    assert(pArea != nullptr);

    // Resize
    pTSNumberRules.reset(1, nbYears);
    return true;
}

void ReservesNeedTSNumberData::setAreaTSnumber(const uint year, uint value)
{
    if (year < pTSNumberRules.height)
    {
        pTSNumberRules[0][year] = value;
    }
}

bool ReservesNeedTSNumberData::apply(Study& study)
{
    bool ret = true;
    Yuni::CString<512, false> logprefix;
    // Errors
    uint errors = 0;

    assert(pArea != nullptr);
    assert(pArea->index < study.areas.size());
    Area& area = *(study.areas.byIndex[pArea->index]);

    if (!area.allCapacityReservations)
    {
        return ret;
    }

    const MatrixType::ColumnType& col = pTSNumberRules[0];
    for (const auto& [reserveID, reserve]: area.allCapacityReservations->areaCapacityReservations)
    {
        logprefix.clear() << "Reserves Need: Area '" << area.name << "', Reserve '" << reserveID
                          << "': ";
        ret = ApplyToMatrix(errors, logprefix, reserve.need, col, 0) && ret;
    }
    return ret;
}
} // namespace Antares::Data::ScenarioBuilder
