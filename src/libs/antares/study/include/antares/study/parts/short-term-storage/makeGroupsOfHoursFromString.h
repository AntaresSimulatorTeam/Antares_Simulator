// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <set>
#include <stdexcept>
#include <string>
#include <vector>

namespace Antares::Data::ShortTermStorage
{
struct ShortTermStorageAdditionalConstraintsError final: std::invalid_argument
{
    using std::invalid_argument::invalid_argument;
};

std::vector<std::set<int>> makeGroupsOfHours(const std::string& hoursField);

} // namespace Antares::Data::ShortTermStorage
