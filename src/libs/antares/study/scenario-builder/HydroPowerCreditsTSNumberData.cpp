

#include "HydroPowerCreditsTSNumberData.h"
#include "applyToMatrix.hxx"

// ================================
// Hydro Energy Credits...
// ================================

uint hydroPowerCreditsTSNumberData::get_tsGenCount(const Study& study) const
{
    //This function must be overriden because it is inherited from abstract class
    return 0;
}

bool hydroPowerCreditsTSNumberData::apply(Study& study)
{
    bool ret = true;
    CString<512, false> logprefix;
    // Errors
    uint errors = 0;

    // The total number of areas;
    const uint areaCount = study.areas.size();

    const uint tsGenCountHydroPowerCredits = get_tsGenCount(study);

    for (uint areaIndex = 0; areaIndex != areaCount; ++areaIndex)
    {
        // Alias to the current area
        Area& area = *(study.areas.byIndex[areaIndex]);
        // alias to the current column
        assert(areaIndex < pTSNumberRules.width);
        const MatrixType::ColumnType& col = pTSNumberRules[areaIndex];

        logprefix.clear() << "Hydro Power Credits: Area '" << area.name << "': ";
        ret = ApplyToMatrixPowerCredits(errors, logprefix, *area.hydro.series, col, tsGenCountHydroPowerCredits) && ret;
    }
    return ret;
}