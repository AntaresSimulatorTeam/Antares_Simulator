// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/study/scenario-builder/HydroFinalLevelsData.h"

namespace Antares::Data::ScenarioBuilder
{

bool HydroFinalLevelsData::apply(Study& study)
{
    study.scenarioFinalHydroLevels.copyFrom(pHydroLevelsRules);
    return true;
}

} // namespace Antares::Data::ScenarioBuilder
