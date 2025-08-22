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

namespace Antares::Optimization
{
/**
 * @brief Full variable key including optional scenario and timestep dimensions.
 *
 * Combines a mandatory (component, variable) pair (interned via PartialKey) with
 * optional scenario (Monte Carlo year) and timestep indices.
 * Provides strong ordering via spaceship operator for use in ordered containers if needed.
 */
class FullKey
{
public:
    /** Scalar constructor (no scenario/time dependence). */
    FullKey(const std::string& component, const std::string& variable, StringToIdMapper& mapper);
    /** Scenario-dependent (no time). */
    FullKey(const std::string& component,
            const std::string& variable,
            MCYearAndTime::MCYear scenario,
            StringToIdMapper& mapper);
    /** Scenario + timestep dependent. */
    FullKey(const std::string& component,
            const std::string& variable,
            MCYearAndTime::MCYear scenario,
            unsigned int timestep,
            StringToIdMapper& mapper);

    [[nodiscard]] const PartialKey& getPartialKey() const; ///< Access interned pair
    [[nodiscard]] const std::string& getComponent() const; ///< Convenience pass-through
    [[nodiscard]] const std::string& getVariable() const;  ///< Convenience pass-through
    [[nodiscard]] std::optional<MCYearAndTime::MCYear> getScenario()
      const;                                                       ///< Scenario index (if any)
    [[nodiscard]] std::optional<unsigned int> getTimestep() const; ///< Timestep index (if any)

    std::strong_ordering operator<=>(const FullKey&) const noexcept;
    bool operator==(const FullKey& other) const noexcept;

private:
    PartialKey pk;
    std::optional<MCYearAndTime::MCYear> scenario;
    std::optional<unsigned int> timestep;
};

inline std::ostream& operator<<(std::ostream& os, const FullKey& key)
{
    return os << fmt::format("FullKey({}, {}, {}, {})",
                             key.getComponent(),
                             key.getVariable(),
                             key.getScenario().has_value()
                               ? std::to_string(format_as(*key.getScenario()))
                               : "nullopt",
                             key.getTimestep().has_value() ? std::to_string(*key.getTimestep())
                                                           : "nullopt");
}

/**
 * @brief Hash functor for FullKey (combines hash of PartialKey and optionals).
 */
class FullKeyHash
{
public:
    std::size_t operator()(const FullKey& p) const;
};
} // namespace Antares::Optimization
