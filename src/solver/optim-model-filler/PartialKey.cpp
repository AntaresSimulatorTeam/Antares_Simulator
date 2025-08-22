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

#include <boost/container_hash/hash.hpp>

#include <antares/solver/optim-model-filler/PartialKey.h>

namespace Antares::Optimization
{
PartialKey::PartialKey(std::string_view component_name,
                       std::string_view variable_name,
                       StringToIdMapper& mapper) noexcept:
    component_id_(mapper.id(component_name)),
    variable_id_(mapper.id(variable_name)),
    mapper_(mapper)
{
}

const std::string& PartialKey::getComponent() const noexcept
{
    return mapper_.get(component_id_);
}

const std::string& PartialKey::getVariable() const noexcept
{
    return mapper_.get(variable_id_);
}

StringToIdMapper::id_type PartialKey::getComponentId() const noexcept
{
    return component_id_;
}

StringToIdMapper::id_type PartialKey::getVariableId() const noexcept
{
    return variable_id_;
}

auto PartialKey::operator<=>(const PartialKey& other) const
{
    return std::tie(component_id_, variable_id_)
           <=> std::tie(other.component_id_, other.variable_id_);
}

bool PartialKey::operator==(const PartialKey& other) const
{
    return (component_id_ == other.component_id_) && (variable_id_ == other.variable_id_);
}

std::size_t PartialKeyHash::operator()(const PartialKey& p) const noexcept
{
    std::size_t h1 = std::hash<StringToIdMapper::id_type>{}(p.getComponentId());
    std::size_t h2 = std::hash<StringToIdMapper::id_type>{}(p.getVariableId());
    return h1 ^ (h2 << 1);
}

} // namespace Antares::Optimization
