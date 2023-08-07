//
// Created by marechaljas on 04/07/23.
//

#include "HydroTSNumberData.h"
#include "applyToMatrix.hxx"

namespace Antares::Data::ScenarioBuilder
{
uint hydroTSNumberData::get_tsGenCount(const Study& study) const
{
    // General data
    auto& parameters = study.parameters;

    const bool tsGenHydro = (0 != (parameters.timeSeriesToGenerate & timeSeriesHydro));
    return tsGenHydro ? parameters.nbTimeSeriesHydro : 0u;
}

bool hydroTSNumberData::apply(Study& study)
{
    bool ret = true;
    CString<512, false> logprefix;
    // Errors
    uint errors = 0;

    // The total number of areas;
    const uint areaCount = study.areas.size();

    const uint tsGenCountHydro = get_tsGenCount(study);

    for (uint areaIndex = 0; areaIndex != areaCount; ++areaIndex)
    {
        // Alias to the current area
        Area& area = *(study.areas.byIndex[areaIndex]);
        // alias to the current column
        assert(areaIndex < pTSNumberRules.width);
        const MatrixType::ColumnType& col = pTSNumberRules[areaIndex];

        logprefix.clear() << "Hydro: Area '" << area.name << "': ";
        ret = ApplyToMatrix(errors, logprefix, *area.hydro.series, col, tsGenCountHydro) && ret;
    }
    return ret;
}

}