// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/study/scenario-builder/HydroLevelsData.h"

#include "antares/study/scenario-builder/scBuilderUtils.h"

namespace Antares::Data::ScenarioBuilder
{

bool HydroLevelsData::reset(const Study& study)
{
    const unsigned int nbYears = study.parameters.nbYears;

    pHydroLevelsRules.reset(study.areas.size(), nbYears);
    pHydroLevelsRules.fill(std::nan(""));

    return true;
}

void HydroLevelsData::setTSnumber(unsigned int areaindex, unsigned int year, double value)
{
    if (areaindex < pHydroLevelsRules.width && year < pHydroLevelsRules.height)
    {
        pHydroLevelsRules[areaindex][year] = value;
    }
}

unsigned int HydroLevelsData::width() const
{
    return pHydroLevelsRules.width;
}

unsigned int HydroLevelsData::height() const
{
    return pHydroLevelsRules.height;
}

double HydroLevelsData::get_value(unsigned int x, unsigned int y) const
{
    return pHydroLevelsRules.entry[y][x];
}

void HydroLevelsData::set_value(unsigned int x, unsigned int y, double value)
{
    pHydroLevelsRules.entry[y][x] = value;
}

} // namespace Antares::Data::ScenarioBuilder
