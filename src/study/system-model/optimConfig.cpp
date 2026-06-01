// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/study/system-model/optimConfig.h"

namespace Antares::Solver::Config
{

const char* LocationToStr(Location loc)
{
    switch (loc)
    {
    case Location::MASTER:
        return "master";
    case Location::MASTER_AND_SUBPROBLEMS:
        return "master-and-subproblems";
    case Location::SUBPROBLEMS:
        return "subproblems";
    default:
        return "Unknown";
    }
}

bool isInSubProblem(const Location& l)
{
    return l == Location::SUBPROBLEMS || l == Location::MASTER_AND_SUBPROBLEMS;
}

bool isInMasterProblem(const Location& l)
{
    return l == Location::MASTER || l == Location::MASTER_AND_SUBPROBLEMS;
}

bool AreLocationsCompatibleForFillers(Location lhs, Location rhs)
{
    switch (rhs)
    {
    case Location::MASTER:
        return lhs == Location::MASTER || lhs == Location::MASTER_AND_SUBPROBLEMS;
    case Location::SUBPROBLEMS:
        return lhs == Location::SUBPROBLEMS || lhs == Location::MASTER_AND_SUBPROBLEMS;
    case Location::MASTER_AND_SUBPROBLEMS:
        return true;
    default:
        return false;
    }
}

// stricter for MASTER_AND_SUBPROLEMS
bool AreLocationsCompatibleForExpressions(Location lhs, Location rhs)
{
    switch (rhs)
    {
    case Location::MASTER:
        return lhs == Location::MASTER || lhs == Location::MASTER_AND_SUBPROBLEMS;
    case Location::SUBPROBLEMS:
        return lhs == Location::SUBPROBLEMS || lhs == Location::MASTER_AND_SUBPROBLEMS;
    case Location::MASTER_AND_SUBPROBLEMS:
        return lhs == Location::MASTER_AND_SUBPROBLEMS;
    default:
        return false;
    }
}

} // namespace Antares::Solver::Config
