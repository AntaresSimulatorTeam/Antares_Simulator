// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

namespace Antares::Solver::Config
{

enum class Location
{
    MASTER,
    MASTER_AND_SUBPROBLEMS,
    SUBPROBLEMS
};

const char* LocationToStr(Location loc);

bool AreLocationsCompatibleForFillers(Location lhs, Location rhs);
// stricter for MASTER_AND_SUBPROLEMS
bool AreLocationsCompatibleForExpressions(Location lhs, Location rhs);

} // namespace Antares::Solver::Config
