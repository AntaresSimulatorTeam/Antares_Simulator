// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/study/scenario-builder/TSnumberData.h"

#include "antares/study/scenario-builder/applyToMatrix.hxx"
#include "antares/study/scenario-builder/scBuilderUtils.h"

namespace Antares::Data::ScenarioBuilder
{

bool TSNumberData::reset(const Study& study)
{
    const uint nbYears = study.parameters.nbYears;

    // Standard timeseries (load, wind, ...)
    pTSNumberRules.reset(study.areas.size(), nbYears);
    return true;
}

#ifdef BUILD_UI
void TSNumberData::saveToINIFile(const Study& study, Yuni::IO::File::Stream& file) const
{
    // Prefix
    CString<512, false> prefix;
    prefix += get_prefix();

    // Foreach year
    assert(pTSNumberRules.width == study.areas.size());
    for (uint index = 0; index != pTSNumberRules.width; ++index)
    {
        // alias to the current column
        const MatrixType::ColumnType& col = pTSNumberRules[index];
        // Foreach area...
        for (uint y = 0; y != pTSNumberRules.height; ++y)
        {
            const MatrixType::Type value = col[y];
            // Equals to zero means 'auto', which is the default mode
            if (!value)
            {
                continue;
            }
            assert(index < study.areas.size());
            file << prefix << study.areas.byIndex[index]->id << ',' << y << " = " << value << '\n';
        }
    }
}
#endif

void TSNumberData::setTSnumber(uint areaindex, uint year, uint value)
{
    assert(areaindex < pTSNumberRules.width);
    if (year < pTSNumberRules.height)
    {
        pTSNumberRules[areaindex][year] = value;
    }
}

void TSNumberData::set_value(uint x, uint y, uint value) const
{
    pTSNumberRules.entry[y][x] = value;
}

} // namespace Antares::Data::ScenarioBuilder
