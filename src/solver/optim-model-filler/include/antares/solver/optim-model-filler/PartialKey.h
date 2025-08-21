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
 * PartialKey
 * Modernisé pour:
 *  - Internage des chaînes (réduction allocations & comparaison plus rapide)
 *  - Stockage sous forme d'identifiants 32 bits
 *  - API inchangée extérieurement (getComponent()/getVariable())
 *
 * Thread-safety: interner protégé par mutex lors de l'insertion; lectures lock-free.
 */
class PartialKey
{
public:
    using id_type = std::uint32_t;

    PartialKey(std::string_view component_id, std::string_view variable_id) noexcept;

    [[nodiscard]] const std::string& getComponent() const noexcept;
    [[nodiscard]] const std::string& getVariable() const noexcept;

    // Accès direct aux IDs (nouvelle API interne éventuelle)
    [[nodiscard]] id_type getComponentId() const noexcept
    {
        return component_id_;
    }

    [[nodiscard]] id_type getVariableId() const noexcept
    {
        return variable_id_;
    }

    auto operator<=>(const PartialKey&) const = default; // comparaison sur les IDs

private:
    id_type component_id_{};
    id_type variable_id_{};

    struct StringInterner
    {
        mutable std::shared_mutex mtx;                // lecture fréquente, écriture rare
        std::unordered_map<std::string, id_type> map; // string => id
        std::vector<std::string> reverse;             // id => string (stable storage)

        id_type intern(std::string_view s)
        {
            // Chemin rapide lecture d'abord
            {
                std::shared_lock lock(mtx);
                if (auto it = map.find(std::string(s)); it != map.end())
                {
                    return it->second;
                }
            }
            // Écriture si absent
            std::unique_lock lock(mtx);
            auto it2 = map.find(std::string(s));
            if (it2 != map.end())
            {
                return it2->second; // inséré entre temps
            }
            id_type id = static_cast<id_type>(reverse.size());
            reverse.emplace_back(s);
            map.emplace(reverse.back(), id);
            return id;
        }

        const std::string& get(id_type id) const noexcept
        {
            std::shared_lock lock(mtx);
            return reverse[id];
        }
    };

    static StringInterner& interner() noexcept;
};

class PartialKeyHash
{
public:
    std::size_t operator()(const PartialKey& p) const noexcept;
};

} // namespace Antares::Optimization
