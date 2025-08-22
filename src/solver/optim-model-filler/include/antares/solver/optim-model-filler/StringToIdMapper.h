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
#include <shared_mutex>
#include <unordered_map>
#include <vector>

namespace Antares::Optimization
{
class StringToIdMapper
{
public:
    using id_type = std::uint32_t;

    /**
     * @brief Intern string (idempotent). O(log N) average with unordered_map.
     * @param s The string to intern.
     * @return Existing or newly created id.
     */
    id_type id(std::string_view s);

    /** @return Reference to interned string for given id (undefined if id invalid). */
    const std::string& get(id_type id) const noexcept;

private:
    mutable std::shared_mutex mtx;                ///< Guards map and reverse
    std::unordered_map<std::string, id_type> map; ///< String to id
    std::vector<std::string> reverse;             ///< Id to string
};
} // namespace Antares::Optimization
