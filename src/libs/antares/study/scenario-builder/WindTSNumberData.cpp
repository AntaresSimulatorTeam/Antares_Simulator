//
// Created by marechaljas on 04/07/23.
//
#include "WindTSNumberData.h"
#include "applyToMatrix.hxx"

namespace Antares::Data::ScenarioBuilder
{
uint windTSNumberData::get_tsGenCount(const Study& study) const
{
    // General data
    auto& parameters = study.parameters;

    const bool tsGenWind = (0 != (parameters.timeSeriesToGenerate & timeSeriesWind));
    return tsGenWind ? parameters.nbTimeSeriesWind : 0u;
}

bool windTSNumberData::apply(/*const*/ Study& study)
{
    bool ret = true;
    CString<512, false> logprefix;
    // Errors
    uint errors = 0;

    // The total number of areas;
    const uint areaCount = study.areas.size();

    const uint tsGenCountWind = get_tsGenCount(study);

    for (uint areaIndex = 0; areaIndex != areaCount; ++areaIndex)
    {
        // Alias to the current area
        Area& area = *(study.areas.byIndex[areaIndex]);
        // alias to the current column
        assert(areaIndex < pTSNumberRules.width);
        const MatrixType::ColumnType& col = pTSNumberRules[areaIndex];

        logprefix.clear() << "Wind: area '" << area.name << "': ";
        ret = ApplyToMatrix(errors, logprefix, *area.wind.series, col, tsGenCountWind) && ret;
    }
    return ret;
}
} // namespace Antares::Data::ScenarioBuilder