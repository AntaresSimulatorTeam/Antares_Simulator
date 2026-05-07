// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/study/scenario-builder/hydroLevelsData.h"

#include <iomanip>
#include <sstream>

#include "antares/study/scenario-builder/scBuilderUtils.h"

namespace Antares::Data::ScenarioBuilder
{

hydroLevelsData::hydroLevelsData(std::function<void(Study&, MatrixType&)> applyToTarget):
    applyToTarget_(applyToTarget)
{
}

bool hydroLevelsData::reset(const Study& study)
{
    const uint nbYears = study.parameters.nbYears;

    pHydroLevelsRules.reset(study.areas.size(), nbYears);
    pHydroLevelsRules.fill(std::nan(""));

    return true;
}

void hydroLevelsData::set_value(uint x, uint y, double value)
{
    pHydroLevelsRules.entry[y][x] = value;
}

bool hydroLevelsData::apply(Study& study)
{
    applyToTarget_(study, pHydroLevelsRules);
    return true;
}

} // namespace Antares::Data::ScenarioBuilder
