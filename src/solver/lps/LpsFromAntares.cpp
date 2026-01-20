// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/lps/LpsFromAntares.h"

#include <fmt/format.h>

#include "antares/exception/RuntimeError.hpp"

namespace Antares::Solver
{
bool LpsFromAntares::empty() const
{
    return constantProblemData.VariablesCount == 0 || weeklyProblems.empty();
}

void LpsFromAntares::setConstantData(const ConstantDataFromAntares& data)
{
    constantProblemData = data;
}

void LpsFromAntares::addWeeklyData(WeeklyProblemId id, WeeklyDataFromAntares&& data)
{
    weeklyProblems.emplace(id, std::move(data));
}

const WeeklyDataFromAntares& LpsFromAntares::weeklyData(WeeklyProblemId id) const
{
    auto it = weeklyProblems.find(id);
    if (it == weeklyProblems.end())
    {
        throw Antares::Error::RuntimeError(
          fmt::format("No data for the requested weekly problem: year {},  week {}",
                      id.year,
                      id.week));
    }
    return it->second;
}

size_t LpsFromAntares::weekCount() const noexcept
{
    return weeklyProblems.size();
}

} // namespace Antares::Solver
