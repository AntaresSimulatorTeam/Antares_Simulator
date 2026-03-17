// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/simulation/apply-scenario.h"

#include <algorithm>
#include <cctype>

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
    std::transform(id.begin(),
                   id.end(),
                   id.begin(),
                   [](unsigned char c) { return std::tolower(c); });

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

    study.scenarioRules.reset();
    logs.info();
}

} // namespace Antares::Solver
