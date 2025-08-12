/*
 * Copyright 2007-2025, RTE (https://www.rte-france.com)
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

#include "MCYearAndTime.h"

template<>
struct boost::hash<Antares::Optimization::PartialKey>
{
    std::size_t operator()(const Antares::Optimization::PartialKey& p) const
    {
        return Antares::Optimization::PartialKeyHash{}(p);
    }
}; // namespace boost

template<>
struct boost::hash<Antares::Optimization::MCYearAndTime>
{
    std::size_t operator()(const Antares::Optimization::MCYearAndTime& p) const
    {
        std::size_t seed = 0;
        boost::hash_combine(seed, p.mcYear);
        boost::hash_combine(seed, p.timestep);
        return seed;
    }
}; // namespace boost

namespace Antares::Optimization
{

class FullKey
{
public:
    FullKey(const std::string& component, const std::string& variable);
    FullKey(const std::string& component, const std::string& variable, MCYearAndTime time);

    [[nodiscard]] const PartialKey& getPartialKey() const;
    [[nodiscard]] const std::string& getComponent() const;
    [[nodiscard]] const std::string& getVariable() const;
    [[nodiscard]] std::optional<MCYearAndTime> getTime() const;

    auto operator<=>(const FullKey&) const = default; // Automatically generates <, >, ==, etc.

private:
    PartialKey pk;
    std::optional<MCYearAndTime> time;
};

class FullKeyHash
{
public:
    std::size_t operator()(const FullKey& p) const;
};
} // namespace Antares::Optimization
