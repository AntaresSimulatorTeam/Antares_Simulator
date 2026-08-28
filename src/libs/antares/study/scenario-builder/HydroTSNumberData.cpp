// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

//
// Created by marechaljas on 04/07/23.
//

#include "antares/study/scenario-builder/HydroTSNumberData.h"

#include "antares/study/scenario-builder/applyToMatrix.hxx"

namespace Antares::Data::ScenarioBuilder
{
unsigned int hydroTSNumberData::get_tsGenCount(const Study& study) const
{
    // General data
    auto& parameters = study.parameters;

    const bool tsGenHydro = (0 != (parameters.timeSeriesToGenerate & timeSeriesHydro));
    return tsGenHydro ? parameters.nbTimeSeriesHydro : 0u;
}

bool hydroTSNumberData::apply(Study& study)
{
    bool ret = true;
    std::string logprefix;
    // Errors
    unsigned int errors = 0;

    // The total number of areas;
    const unsigned int areaCount = study.areas.size();

    const unsigned int tsGenCountHydro = get_tsGenCount(study);

    for (unsigned int areaIndex = 0; areaIndex != areaCount; ++areaIndex)
    {
        // Alias to the current area
        Area& area = *(study.areas.byIndex[areaIndex]);
        // alias to the current column
        assert(areaIndex < pTSNumberRules.width);
        const MatrixType::ColumnType& col = pTSNumberRules[areaIndex];

        logprefix = "Hydro: area '" + area.name + "': ";
        ret = ApplyToMatrix(errors, logprefix, *area.hydro.series, col, tsGenCountHydro) && ret;
    }
    return ret;
}

} // namespace Antares::Data::ScenarioBuilder
