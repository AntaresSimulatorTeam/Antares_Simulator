/*
** Copyright 2007-2024, RTE (https://www.rte-france.com)
** See AUTHORS.txt
** SPDX-License-Identifier: MPL-2.0
** This file is part of Antares-Simulator,
** Adequacy and Performance assessment for interconnected energy networks.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the Mozilla Public Licence 2.0 as published by
** the Mozilla Foundation, either version 2 of the License, or
** (at your option) any later version.
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/

#include "antares/solver/simulation/apply-scenario.h"

#include <antares/study/scenario-builder/sets.h>

namespace Antares::Solver
{
void ApplyCustomScenario(Data::Study& study)
{
    auto& parameters = study.parameters;

    auto& rulename = parameters.activeRulesScenario;
    logs.info() << "Preparing time-series numbers... (" << rulename << ')';
    logs.info() << "  :: Scenario Builder, active target: " << rulename;
    Data::RulesScenarioName id = rulename;
    id.toLower();

    study.scenarioRulesLoadIfNotAvailable();
    if (study.scenarioRules)
    {
        Data::ScenarioBuilder::Rules::Ptr rules = study.scenarioRules->find(id);
        if (rules)
        {
            rules->apply();
        }
        else
        {
            logs.error() << "Scenario Builder: Impossible to find the active ruleset '" << rulename
                         << "'";
        }
    }

    study.scenarioRulesDestroy();
    logs.info();
}

} // namespace Antares::Solver
