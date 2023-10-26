

#include "HydroMaxPowerTSNumberData.h"
#include "applyToMatrix.hxx"

// ================================
// Hydro Max Power...
// ================================

namespace Antares::Data::ScenarioBuilder
{
uint hydroMaxPowerTSNumberData::get_tsGenCount(const Study& study) const
{
    //This function must be overriden because it is inherited from abstract class
    return 0;
}

bool hydroMaxPowerTSNumberData::apply(Study& study)
{
    bool ret = true;
    CString<512, false> logprefix;
    // Errors
    uint errors = 0;

    // The total number of areas;
    const uint areaCount = study.areas.size();

    const uint tsGenCountHydroMaxPower = get_tsGenCount(study);

    for (uint areaIndex = 0; areaIndex != areaCount; ++areaIndex)
    {
        // Alias to the current area
        Area& area = *(study.areas.byIndex[areaIndex]);
        // alias to the current column
        assert(areaIndex < pTSNumberRules.width);
        const MatrixType::ColumnType& col = pTSNumberRules[areaIndex];

        logprefix.clear() << "Hydro Max Power: Area '" << area.name << "': ";
        ret = ApplyToMatrixMaxPower(errors, logprefix, *area.hydro.series, col, tsGenCountHydroMaxPower) && ret;
    }
    return ret;
}
}