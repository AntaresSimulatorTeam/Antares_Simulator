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

#include <cstdint>
#include <shared_mutex>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace Antares::Optimization
{
/**
 * @brief Lightweight interned pair (component, variable) used as part of full variable keys.
 *
 * Stores only two 32-bit identifiers referencing globally interned strings. This reduces
 * memory usage and speeds up comparisons / hashing versus storing full std::string copies.
 *
 * Thread-safety: construction may insert into the global interner guarded by a shared_mutex.
 * Reads (getComponent / getVariable) acquire a shared (read) lock; they are safe concurrently.
 *
 * NOTE: Creating a PartialKey will intern the provided strings if not already present.
 */
class PartialKey
{
public:
    using id_type = std::uint32_t;

    /**
     * @brief Construct and (if needed) intern the provided strings.
     * @param component_id Component identifier (string_view accepted, copied only if new).
     * @param variable_id Variable identifier.
     */
    PartialKey(std::string_view component_id, std::string_view variable_id) noexcept;

    /** @return Component identifier string (stable reference inside interner). */
    [[nodiscard]] const std::string& getComponent() const noexcept;
    /** @return Variable identifier string (stable reference inside interner). */
    [[nodiscard]] const std::string& getVariable() const noexcept;

    /** @return Interned numeric id of component. */
    [[nodiscard]] id_type getComponentId() const noexcept { return component_id_; }
    /** @return Interned numeric id of variable. */
    [[nodiscard]] id_type getVariableId() const noexcept { return variable_id_; }

    /** Three-way comparison based on (component_id_, variable_id_). */
    auto operator<=>(const PartialKey&) const = default;

private:
    id_type component_id_{}; ///< Interned id of component
    id_type variable_id_{};  ///< Interned id of variable

    struct StringInterner
    {
        mutable std::shared_mutex mtx;                    ///< Protects both maps
        std::unordered_map<std::string, id_type> map;     ///< string => id
        std::vector<std::string> reverse;                 ///< id => string

        /**
         * @brief Intern string (idempotent). O(log N) average with unordered_map.
         * @param s The string to intern.
         * @return Existing or newly created id.
         */
        id_type intern(std::string_view s)
        {
            {
                std::shared_lock lock(mtx);
                if (auto it = map.find(std::string(s)); it != map.end())
                    return it->second;
            }
            std::unique_lock lock(mtx);
            auto it2 = map.find(std::string(s));
            if (it2 != map.end())
                return it2->second; // inserted meanwhile
            id_type id = static_cast<id_type>(reverse.size());
            reverse.emplace_back(s);
            map.emplace(reverse.back(), id);
            return id;
        }

        /** @return Reference to interned string for given id (undefined if id invalid). */
        const std::string& get(id_type id) const noexcept
        {
            std::shared_lock lock(mtx);
            return reverse[id];
        }
    };

    /** @return Singleton interner instance. */
    static StringInterner& interner() noexcept;
};

/**
 * @brief Hash functor for PartialKey (uses interned numeric ids only).
 */
class PartialKeyHash
{
public:
    std::size_t operator()(const PartialKey& p) const noexcept;
};

} // namespace Antares::Optimization
