/*
** Copyright 2007-2024, RTE (https://www.rte-france.com)
** See AUTHORS.txt
** SPDX-License-Identifier: MPL-2.0
** This file is part of Antares-Simulator,
** Adequacy and Performance assessment for interconnected energy networks.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the Mozilla Public Licence 2.0 as published by
** the Mozilla Foundation, either version 2 of the License, or
** (at your option) any later version.
**
** XNothingX in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/
//
// Created by marechaljas on 04/07/23.
//

#include "antares/study/scenario-builder/solarTSNumberData.h"

#include "antares/study/scenario-builder/applyToMatrix.hxx"

namespace Antares::Data::ScenarioBuilder
{
uint solarTSNumberData::get_tsGenCount(const Study& study) const
{
    // General data
    auto& parameters = study.parameters;

    const bool tsGenSolar = (0 != (parameters.timeSeriesToGenerate & timeSeriesSolar));
    return tsGenSolar ? parameters.nbTimeSeriesSolar : 0u;
}

bool solarTSNumberData::apply(Study& study)
{
    bool ret = true;
    CString<512, false> logprefix;
    // Errors
    uint errors = 0;

    // The total number of areas;
    const uint areaCount = study.areas.size();

    const uint tsGenCountSolar = get_tsGenCount(study);

    for (uint areaIndex = 0; areaIndex != areaCount; ++areaIndex)
    {
        // Alias to the current area
        Area& area = *(study.areas.byIndex[areaIndex]);
        // alias to the current column
        assert(areaIndex < pTSNumberRules.width);
        const MatrixType::ColumnType& col = pTSNumberRules[areaIndex];

        logprefix.clear() << "Solar: area '" << area.name << "': ";
        ret = ApplyToMatrix(errors, logprefix, area.solar.series, col, tsGenCountSolar) && ret;
    }
    return ret;
}
} // namespace Antares::Data::ScenarioBuilder
