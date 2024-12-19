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
#pragma once

#include <unordered_map>

#include "component.h"

namespace Antares::Study::SystemModel
{

/**
 * Defines the attributes of the System class
 * Made into a struct to avoid duplication in SystemBuilder
 */
struct SystemData
{
};

/**
 * Defines the simulated system.
 */
class System
{
public:
    // Only allowing one private constructor (see below) to forbid empty Systems
    System() = delete;
    System(System& other) = delete;

    const std::string& Id() const
    {
        return id_;
    }

    const std::unordered_map<std::string, Component>& Components() const
    {
        return components_;
    }

private:
    // Only SystemBuilder is allowed to build System instances
    friend class SystemBuilder;
    System(std::string_view id, std::vector<Component> components);
    std::string id_;
    std::unordered_map<std::string, Component> components_;
    std::pair<std::string, Component> makeComponent(Component& component) const;
};

class SystemBuilder
{
public:
    SystemBuilder& withId(std::string_view id);
    SystemBuilder& withComponents(std::vector<Component>& components);
    System build() const;

private:
    std::string id_;
    std::vector<Component> components_;
};

} // namespace Antares::Study::SystemModel
