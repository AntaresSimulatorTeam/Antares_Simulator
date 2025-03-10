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
#pragma once

#include <optional>

#include <boost/container_hash/hash.hpp>

#include <antares/solver/optim-model-filler/PartialKey.h>

template<>
struct boost::hash<Antares::Optimization::PartialKey>
{
    std::size_t operator()(const Antares::Optimization::PartialKey& p) const
    {
        return Antares::Optimization::PartialKeyHash{}(p);
    }
}; // namespace boost

namespace Antares::Optimization
{

class FullKey
{
public:
    FullKey(const std::string& component, const std::string& variable);
    FullKey(const std::string& component,
            const std::string& variable,
            unsigned int scenario,
            unsigned int timestep);

    const PartialKey& getPartialKey() const;
    const std::string& getComponent() const;
    const std::string& getVariable() const;

    std::optional<unsigned int> getScenario() const;
    std::optional<unsigned int> getTimestep() const;

    auto operator<=>(const FullKey&) const = default; // Automatically generates <, >, ==, etc.

private:
    PartialKey pk;
    std::optional<unsigned int> scenario;
    std::optional<unsigned int> timestep;
};

class FullKeyHash
{
public:
    std::size_t operator()(const FullKey& p) const;
};
} // namespace Antares::Optimization
