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

#include "antares/io/inputs/yml-system/converter.h"

#include <algorithm>

#include "antares/io/inputs/yml-system/system.h"
#include "antares/study/system-model/system.h"

using namespace Antares::Study;

namespace Antares::IO::Inputs::SystemConverter
{

class ErrorWhileSplittingLibraryAndModel: public std::runtime_error
{
public:
    explicit ErrorWhileSplittingLibraryAndModel(const std::string& s):
        runtime_error("'.' not found while splitting library and model: " + s)
    {
    }
};

class LibraryNotFound: public std::runtime_error
{
public:
    explicit LibraryNotFound(const std::string& s):
        runtime_error("No library found with this name: " + s)
    {
    }
};

class ModelNotFound: public std::runtime_error
{
public:
    explicit ModelNotFound(const std::string& s):
        runtime_error("No model found with this name: " + s)
    {
    }
};

static std::pair<std::string, std::string> splitLibraryModelString(const std::string& s)
{
    size_t pos = s.find('.');
    if (pos == std::string::npos)
    {
        throw ErrorWhileSplittingLibraryAndModel(s);
    }

    std::string library = s.substr(0, pos);
    std::string model = s.substr(pos + 1);
    return {library, model};
}

static const SystemModel::Model& getModel(const std::vector<SystemModel::Library>& libraries,
                                          const std::string& libraryId,
                                          const std::string& modelId)
{
    auto lib = std::ranges::find_if(libraries,
                                    [&libraryId](const auto& l) { return l.Id() == libraryId; });
    if (lib == libraries.end())
    {
        throw LibraryNotFound(libraryId);
    }

    auto search = lib->Models().find(modelId);
    if (search == lib->Models().end())
    {
        throw ModelNotFound(modelId);
    }

    return search->second;
}

static SystemModel::Component createComponent(const YmlSystem::Component& c,
                                              const std::vector<SystemModel::Library>& libraries)
{
    const auto [libraryId, modelId] = splitLibraryModelString(c.model);
    SystemModel::ModelBuilder model_builder;

    const SystemModel::Model& model = getModel(libraries, libraryId, modelId);

    SystemModel::ComponentBuilder component_builder;

    std::map<std::string, double> parameters;
    for (const auto& p: c.parameters)
    {
        parameters.try_emplace(p.id, p.value);
    }

    auto component = component_builder.withId(c.id)
                       .withModel(&model)
                       .withScenarioGroupId(c.scenarioGroup)
                       .withParameterValues(parameters)
                       .build();
    return component;
}

SystemModel::System convert(const YmlSystem::System& ymlSystem,
                            const std::vector<SystemModel::Library>& libraries)
{
    std::vector<SystemModel::Component> components;
    for (const auto& c: ymlSystem.components)
    {
        components.push_back(createComponent(c, libraries));
    }

    SystemModel::SystemBuilder builder;
    return builder.withId(ymlSystem.id).withComponents(components).build();
}

} // namespace Antares::IO::Inputs::SystemConverter
