
// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once
#include <fmt/format.h>
#include <ostream>

namespace Antares::Optimization
{
/**
 * Association of a Scenario (year) and time step
 */
struct MCYearAndTime
{
    enum class MCYear : unsigned int
    {
    };
    MCYear mcYear;
    unsigned int timestep;
};

inline auto format_as(const MCYearAndTime::MCYear& mcyear)
{
    return fmt::underlying(mcyear);
}

inline std::ostream& operator<<(std::ostream& os, const MCYearAndTime::MCYear& mcYear)
{
    os << static_cast<unsigned int>(mcYear);
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const MCYearAndTime& st)
{
    os << fmt::format("{{scenario: {}, timestep: {}}}", st.mcYear, st.timestep);
    return os;
}

} // namespace Antares::Optimization
