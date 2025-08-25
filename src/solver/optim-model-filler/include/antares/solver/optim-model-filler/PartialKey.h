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
#include <string>
#include <string_view>

#include "antares/solver/optim-model-filler/StringToIdMapper.h"

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
    /**
     * @brief Construct and (if needed) intern the provided strings.
     * @param component_name Component identifier (string_view accepted, copied only if new).
     * @param variable_name Variable identifier.
     * @param mapper to get id from names
     */
    PartialKey(std::string_view component_name,
               std::string_view variable_name,
               StringToIdMapper& mapper) noexcept;

    // Rule of Five
    ~PartialKey() = default;

    PartialKey(const PartialKey& other) noexcept;

    PartialKey& operator=(const PartialKey& other) noexcept;

    PartialKey(PartialKey&& other) noexcept;

    PartialKey& operator=(PartialKey&& other) noexcept;

    /** @return Component identifier string (stable reference inside interner). */
    [[nodiscard]] const std::string& getComponent() const noexcept;
    /** @return Variable identifier string (stable reference inside interner). */
    [[nodiscard]] const std::string& getVariable() const noexcept;

    /** @return Interned numeric id of component. */
    [[nodiscard]] StringToIdMapper::id_type getComponentId() const noexcept;

    /** @return Interned numeric id of variable. */
    [[nodiscard]] StringToIdMapper::id_type getVariableId() const noexcept;

    /** Three-way comparison based on (component_id_, variable_id_). */
    std::strong_ordering operator<=>(const PartialKey&) const noexcept;
    bool operator==(const PartialKey& other) const noexcept;

private:
    StringToIdMapper::id_type component_id_{}; ///< Interned id of component
    StringToIdMapper::id_type variable_id_{};  ///< Interned id of variable
    StringToIdMapper& mapper_;
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
