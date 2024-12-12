/*
 * Copyright 2007-2024, RTE (https://www.rte-france.com)
 * See AUTHORS.txt
 * SPDX-License-Identifier: MPL-2.0
 * This file is part of Antares-Simulator,
 * Adequacy and Performance assessment for interconnected energy networks.
 *
 * Antares_Simulator is free software: you can redistribute it and/or modify
 * it under the terms of the Mozilla Public Licence 2.0 as published by
 * the Mozilla Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * Antares_Simulator is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * Mozilla Public Licence 2.0 for more details.
 *
 * You should have received a copy of the Mozilla Public Licence 2.0
 * along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
 */

#include "antares/solver/systemParser/system.h"
#include "antares/study/system-model/system.h"

using namespace Antares::Study;

namespace Antares::Solver::SystemConverter
{

static SystemModel::Component createComponent(const SystemParser::Component& c)
{
    SystemModel::ModelBuilder model_builder;
    auto model = model_builder.withId(c.model).build();
    SystemModel::ComponentBuilder component_builder;

    /* std::map<std::string, double> parameters; */
    /* for (const auto& p : c.parameters) */
    /* { */
    /*     parameters.try_emplace(p.id, p.value); */
    /* } */

    auto component = component_builder.withId(c.id)
                       .withModel(&model)
                       .withScenarioGroupId(c.scenarioGroup)
                       /* .withParameterValues(parameters) */
                       .build();
    return component;
}

SystemModel::System convert(const SystemParser::System& parserSystem)
{
    std::vector<SystemModel::Component> components;
    for (const auto& c: parserSystem.components)
    {
        components.push_back(createComponent(c));
    }

    SystemModel::SystemBuilder builder;
    return builder.withId(parserSystem.id).withComponents(components).build();
}

} // namespace Antares::Solver::SystemConverter
