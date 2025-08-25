
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

#include "antares/solver/optim-model-filler/StringToIdMapper.h"

namespace Antares::Optimization
{
StringToIdMapper::StringToIdMapper(const StringToIdMapper& other)
{
    std::shared_lock lock(other.mtx);
    map = other.map;
    reverse = other.reverse;
}

StringToIdMapper& StringToIdMapper::operator=(const StringToIdMapper& other)
{
    if (this != &other)
    {
        // Lock both mutexes in a consistent order to avoid deadlock
        std::shared_lock other_lock(other.mtx);
        std::unique_lock this_lock(mtx);
        map = other.map;
        reverse = other.reverse;
    }
    return *this;
}

StringToIdMapper::StringToIdMapper(StringToIdMapper&& other) noexcept
{
    std::unique_lock lock(other.mtx);
    map = std::move(other.map);
    reverse = std::move(other.reverse);
}

StringToIdMapper& StringToIdMapper::operator=(StringToIdMapper&& other) noexcept
{
    if (this != &other)
    {
        // Lock both mutexes in a consistent order to avoid deadlock
        std::unique_lock other_lock(other.mtx);
        std::unique_lock this_lock(mtx);
        map = std::move(other.map);
        reverse = std::move(other.reverse);
    }
    return *this;
}

StringToIdMapper::id_type StringToIdMapper::id(std::string_view s)
{
    // Different thread can read at the same time
    {
        std::shared_lock lock(mtx);
        if (auto it = map.find(std::string(s)); it != map.end())
        {
            return it->second;
        }
    }
    // Wait for shared_lock (reading) to release before modifying
    std::unique_lock lock(mtx);
    auto it2 = map.find(std::string(s));
    if (it2 != map.end())
    {
        return it2->second; // inserted meanwhile
    }
    id_type id = static_cast<id_type>(reverse.size());
    reverse.emplace_back(s);
    map.emplace(reverse.back(), id);
    return id;
}

const std::string& StringToIdMapper::get(id_type id) const noexcept
{
    std::shared_lock lock(mtx);
    return reverse[id];
}
} // namespace Antares::Optimization
