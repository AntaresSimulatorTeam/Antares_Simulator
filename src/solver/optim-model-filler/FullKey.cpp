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

#include <antares/solver/optim-model-filler/FullKey.h>

namespace Antares::Optimization
{

// FullKey
FullKey::FullKey(const std::string& component,
                 const std::string& variable,
                 unsigned int scenario,
                 unsigned int timestep):
    pk(component, variable),
    scenario(scenario),
    timestep(timestep)
{
}

FullKey::FullKey(const std::string& component, const std::string& variable):
    pk(component, variable)
{
}

const PartialKey& FullKey::getPartialKey() const
{
    return pk;
}

const std::string& FullKey::getComponent() const
{
    return pk.getComponent();
}

const std::string& FullKey::getVariable() const
{
    return pk.getVariable();
}

std::optional<unsigned int> FullKey::getScenario() const
{
    return scenario;
}

std::optional<unsigned int> FullKey::getTimestep() const
{
    return timestep;
}

std::size_t FullKeyHash::operator()(const FullKey& p) const
{
    std::size_t seed = 0;
    boost::hash_combine(seed, p.getPartialKey());
    boost::hash_combine(seed, p.getScenario());
    boost::hash_combine(seed, p.getTimestep());
    return seed;
}

// FullKeyhash

} // namespace Antares::Optimization
