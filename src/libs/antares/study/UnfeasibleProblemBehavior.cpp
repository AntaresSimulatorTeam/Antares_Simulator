// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include <initializer_list>
#include <string>

#include <antares/study/UnfeasibleProblemBehavior.hpp>
#include "antares/enums/Enum.hpp"

namespace Antares::Data
{
bool exportMPS(const UnfeasibleProblemBehavior& unfeasibleProblemBehavior)
{
    switch (unfeasibleProblemBehavior)
    {
    case UnfeasibleProblemBehavior::WARNING_DRY:
    case UnfeasibleProblemBehavior::ERROR_DRY:
        return false;
    case UnfeasibleProblemBehavior::WARNING_MPS:
    case UnfeasibleProblemBehavior::ERROR_MPS:
        return true;
    default:
        throw std::runtime_error(
          "Invalid UnfeasibleProblemBehavior "
          + std::to_string(static_cast<unsigned long>(unfeasibleProblemBehavior)));
        return "";
    }
}

bool stopSimulation(const UnfeasibleProblemBehavior& unfeasibleProblemBehavior)
{
    switch (unfeasibleProblemBehavior)
    {
    case UnfeasibleProblemBehavior::WARNING_DRY:
    case UnfeasibleProblemBehavior::WARNING_MPS:
        return false;
    case UnfeasibleProblemBehavior::ERROR_MPS:
    case UnfeasibleProblemBehavior::ERROR_DRY:
        return true;
    default:
        throw std::runtime_error(
          "Invalid UnfeasibleProblemBehavior "
          + std::to_string(static_cast<unsigned long>(unfeasibleProblemBehavior)));
        return "";
    }
}

namespace Enum
{
template<>
const std::initializer_list<std::string>& getNames<UnfeasibleProblemBehavior>()
{
    // Enum must be stored in lower case and without spaces because values  are trimmed and lowered
    // in ini load
    static std::initializer_list<std::string> s_unfeasibleProblemBehaviorNames{"warning-dry",
                                                                               "warning-verbose",
                                                                               "error-dry",
                                                                               "error-verbose"};
    return s_unfeasibleProblemBehaviorNames;
}
} // namespace Enum

} // namespace Antares::Data
