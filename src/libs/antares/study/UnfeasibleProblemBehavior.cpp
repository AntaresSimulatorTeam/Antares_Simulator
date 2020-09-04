/*
** Copyright 2007-2018 RTE
** Authors: Antares_Simulator Team
**
** This file is part of Antares_Simulator.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** There are special exceptions to the terms and conditions of the
** license as they are applied to this software. View the full text of
** the exceptions in file COPYING.txt in the directory of this software
** distribution
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Antares_Simulator. If not, see <http://www.gnu.org/licenses/>.
**
** SPDX-License-Identifier: licenceRef-GPL3_WITH_RTE-Exceptions
*/

#include <initializer_list>
#include <string>

#include <antares/exception/AssertionError.hpp>
#include <antares/study/UnfeasibleProblemBehavior.hpp>

#include <antares/Enum.hpp>

namespace Antares {
    
namespace Data {

    const char* getIcon(const UnfeasibleProblemBehavior& unfeasibleProblemBehavior) {
        switch (unfeasibleProblemBehavior) {
        case UnfeasibleProblemBehavior::WARNING_DRY:
            return "images/16x16/light_green.png";
        case UnfeasibleProblemBehavior::WARNING_MPS:
            return "images/16x16/light_green.png";
        case UnfeasibleProblemBehavior::ERROR_DRY:
            return "images/16x16/light_orange.png";
        case UnfeasibleProblemBehavior::ERROR_MPS:
            return "images/16x16/light_orange.png";
        default:
            throw AssertionError("Invalid UnfeasibleProblemBehavior " + std::to_string(static_cast<unsigned long>(unfeasibleProblemBehavior)));
            return "";
        }
    }

    bool exportMPS(const UnfeasibleProblemBehavior& unfeasibleProblemBehavior) {
        switch (unfeasibleProblemBehavior) {
        case UnfeasibleProblemBehavior::WARNING_DRY:
        case UnfeasibleProblemBehavior::ERROR_DRY:
            return false;
        case UnfeasibleProblemBehavior::WARNING_MPS:
        case UnfeasibleProblemBehavior::ERROR_MPS:
            return true;
        default:
            throw AssertionError("Invalid UnfeasibleProblemBehavior " + std::to_string(static_cast<unsigned long>(unfeasibleProblemBehavior)));
            return "";
        }
    }

    bool stopSimulation(const UnfeasibleProblemBehavior& unfeasibleProblemBehavior) {
        switch (unfeasibleProblemBehavior) {
        case UnfeasibleProblemBehavior::WARNING_DRY:
        case UnfeasibleProblemBehavior::WARNING_MPS:
            return false;
        case UnfeasibleProblemBehavior::ERROR_MPS:
        case UnfeasibleProblemBehavior::ERROR_DRY:
            return true;
        default:
            throw AssertionError("Invalid UnfeasibleProblemBehavior " + std::to_string(static_cast<unsigned long>(unfeasibleProblemBehavior)));
            return "";
        }
    }    

    std::string getDisplayName(const UnfeasibleProblemBehavior& unfeasibleProblemBehavior) {
        switch (unfeasibleProblemBehavior) {
        case UnfeasibleProblemBehavior::WARNING_DRY:
            return "Warning Dry";
        case UnfeasibleProblemBehavior::WARNING_MPS:
            return "Warning Verbose";
        case UnfeasibleProblemBehavior::ERROR_DRY:
            return "Error Dry";
        case UnfeasibleProblemBehavior::ERROR_MPS:
            return "Error Verbose";
        default:
            throw AssertionError("Invalid UnfeasibleProblemBehavior " + std::to_string(static_cast<unsigned long>(unfeasibleProblemBehavior)));
            return "";

        }
    }

namespace Enum {

    template <>
    const std::initializer_list<std::string>& getNames<UnfeasibleProblemBehavior>() {

        //Enum must be stored in lower case and without spaces because values  are trimmed and lowered in ini load
        static std::initializer_list<std::string> s_unfeasibleProblemBehaviorNames{
            "warning-dry",
            "warning-verbose",
            "error-dry",
            "error-verbose"
        };
        return s_unfeasibleProblemBehaviorNames;
    }
} // namespace Enum
   
} // namespace Data

} // namespace Antares