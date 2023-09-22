//
// Created by marechaljas on 04/07/23.
//

#include "LoadTSNumberData.h"
#include "applyToMatrix.hxx"

namespace Antares::Data::ScenarioBuilder
{
uint loadTSNumberData::get_tsGenCount(const Study& study) const
{
    // General data
    auto& parameters = study.parameters;

    const bool tsGenLoad = (0 != (parameters.timeSeriesToGenerate & timeSeriesLoad));
    return tsGenLoad ? parameters.nbTimeSeriesLoad : 0u;
}

bool loadTSNumberData::apply(Study& study)
{
    bool ret = true;
    CString<512, false> logprefix;
    // Errors
    uint errors = 0;

    // The total number of areas;
    const uint areaCount = study.areas.size();

    const uint tsGenCountLoad = get_tsGenCount(study);

    for (uint areaIndex = 0; areaIndex != areaCount; ++areaIndex)
    {
        // Alias to the current area
        Area& area = *(study.areas.byIndex[areaIndex]);
        // alias to the current column
        assert(areaIndex < pTSNumberRules.width);
        const MatrixType::ColumnType& col = pTSNumberRules[areaIndex];

        logprefix.clear() << "Load: Area '" << area.name << "': ";
        ret = ApplyToMatrix(errors, logprefix, *area.load.series, col, tsGenCountLoad) && ret;
    }
    return ret;
}
} // namespace Antares::Data::ScenarioBuilder