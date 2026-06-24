// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/study/scenario-builder/HydroInitialLevelsData.h"

namespace Antares::Data::ScenarioBuilder
{

bool HydroInitialLevelsData::apply(Study& study)
{
    study.scenarioInitialHydroLevels.copyFrom(pHydroLevelsRules);
    return true;
}

} // namespace Antares::Data::ScenarioBuilder
