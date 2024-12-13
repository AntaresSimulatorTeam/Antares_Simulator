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

#include "antares/solver/systemParser/converter.h"

#include "antares/solver/systemParser/system.h"
#include "antares/study/system-model/system.h"

using namespace Antares::Study;

namespace Antares::Solver::SystemConverter
{

static std::pair<std::string, std::string> splitLibraryModelString(const std::string& s)
{
    size_t pos = s.find('.');
    if (pos == std::string::npos)
    {
        throw std::runtime_error("Error while splitting library model: " + s +
                                 "Correct format is lirabry.model");
    }

    std::string library = s.substr(0, pos);
    std::string model = s.substr(pos + 1);
    return {library, model};
}

static SystemModel::Component createComponent(const SystemParser::Component& c, const std::vector<SystemModel::Library>& libraries)
{
    const auto [libraryId, modelId] = splitLibraryModelString(c.model);
    SystemModel::ModelBuilder model_builder;
    auto model = std::make_shared<SystemModel::Model>(model_builder.withId(modelId).build());
    SystemModel::ComponentBuilder component_builder;

    std::map<std::string, double> parameters;
    for (const auto& p : c.parameters)
    {
        parameters.try_emplace(p.id, p.value);
    }

    auto component = component_builder.withId(c.id)
                       .withModel(model.get())
                       .withScenarioGroupId(c.scenarioGroup)
                       .withParameterValues(parameters)
                       .build();
    return component;
}

SystemModel::System convert(const SystemParser::System& parserSystem, const std::vector<SystemModel::Library>& libraries)
{
    std::vector<SystemModel::Component> components;
    for (const auto& c: parserSystem.components)
    {
        components.push_back(createComponent(c, libraries));
    }

    SystemModel::SystemBuilder builder;
    return builder.withId(parserSystem.id).withComponents(components).build();
}

} // namespace Antares::Solver::SystemConverter
