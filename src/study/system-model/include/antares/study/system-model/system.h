// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <unordered_map>

#include "component.h"

namespace Antares::ModelerStudy::SystemModel
{

/**
 * Defines the simulated system.
 */
class System final
{
public:
    // Only allowing one private constructor (see below) to forbid empty Systems
    System() = delete;
    System(System& other) = delete;
    System(System&& other) = default;

    const std::string& Id() const
    {
        return id_;
    }

    const std::vector<Component>& Components() const
    {
        return components_;
    }

private:
    // Only SystemBuilder is allowed to build System instances
    friend class SystemBuilder;
    System(std::string_view id, std::vector<Component>&& components);
    std::string id_;
    std::vector<Component> components_;
};

class SystemBuilder final
{
public:
    SystemBuilder& withId(std::string_view id);
    SystemBuilder& withComponents(std::vector<Component>&& components);
    System build();

private:
    std::string id_;

    std::vector<Component> components_;
};

} // namespace Antares::ModelerStudy::SystemModel
