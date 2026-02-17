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
