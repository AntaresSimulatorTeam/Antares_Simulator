/*
** Copyright 2007-2024, RTE (https://www.rte-france.com)
** See AUTHORS.txt
** SPDX-License-Identifier: MPL-2.0
** This file is part of Antares-Simulator,
** Adequacy and Performance assessment for interconnected energy networks.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the Mozilla Public Licence 2.0 as published by
** the Mozilla Foundation, either version 2 of the License, or
** (at your option) any later version.
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/

#include <initializer_list>
#include <string>

#include <antares/exception/AssertionError.hpp>
#include <antares/study/UnfeasibleProblemBehavior.hpp>
#include "antares/antares/Enum.hpp"

namespace Antares::Data
{
const char* getIcon(const UnfeasibleProblemBehavior& unfeasibleProblemBehavior)
{
    switch (unfeasibleProblemBehavior)
    {
    case UnfeasibleProblemBehavior::WARNING_DRY:
        return "images/16x16/light_green.png";
    case UnfeasibleProblemBehavior::WARNING_MPS:
        return "images/16x16/light_green.png";
    case UnfeasibleProblemBehavior::ERROR_DRY:
        return "images/16x16/light_orange.png";
    case UnfeasibleProblemBehavior::ERROR_MPS:
        return "images/16x16/light_orange.png";
    default:
        throw AssertionError(
          "Invalid UnfeasibleProblemBehavior "
          + std::to_string(static_cast<unsigned long>(unfeasibleProblemBehavior)));
        return "";
    }
}

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
        throw AssertionError(
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
        throw AssertionError(
          "Invalid UnfeasibleProblemBehavior "
          + std::to_string(static_cast<unsigned long>(unfeasibleProblemBehavior)));
        return "";
    }
}

std::string getDisplayName(const UnfeasibleProblemBehavior& unfeasibleProblemBehavior)
{
    switch (unfeasibleProblemBehavior)
    {
    case UnfeasibleProblemBehavior::WARNING_DRY:
        return "Warning Dry";
    case UnfeasibleProblemBehavior::WARNING_MPS:
        return "Warning Verbose";
    case UnfeasibleProblemBehavior::ERROR_DRY:
        return "Error Dry";
    case UnfeasibleProblemBehavior::ERROR_MPS:
        return "Error Verbose";
    default:
        throw AssertionError(
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
