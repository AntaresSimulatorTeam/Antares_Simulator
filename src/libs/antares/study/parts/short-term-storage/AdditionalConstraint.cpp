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
#include "antares/study/parts/short-term-storage/AdditionalConstraint.h"

#include <antares/logs/logs.h>

namespace Antares::Data::ShortTermStorage
{
bool AdditionalConstraint::validate() const
{
    if (cluster_id.empty())
    {
        error_message = "Cluster ID is empty.";
        return false;
    }

    if (!isValidVariable())
    {
        error_message = "Invalid variable type. Must be 'injection', 'withdrawal', or 'netting'.";
        return false;
    }

    if (!isValidOperatorType())
    {
        error_message = "Invalid operator type. Must be 'less', 'equal', or 'greater'.";
        return false;
    }

    if (!isValidHoursRange())
    {
        error_message = "Hours set contains invalid values. Must be between 1 and 168.";
        return false;
    }

    error_message.clear();
    return true;
}

    bool AdditionalConstraint::isValidHoursRange() const {
// `hours` is a sorted set; begin() gives the smallest and prev(end()) gives the largest.
        return !hours.empty() && *hours.begin() >= 1
               && *std::prev(hours.end()) <= 168;

    }

    bool AdditionalConstraint::isValidVariable() const {
        return variable == "injection" || variable == "withdrawal" || variable == "netting";
    }

    bool AdditionalConstraint::isValidOperatorType() const {
        return operatorType == "less" || operatorType == "equal" || operatorType == "greater";
    }
} // namespace Antares::Data::ShortTermStorage
