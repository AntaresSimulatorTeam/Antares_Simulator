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
        return false;
    }

    if (variable != "injection" && variable != "withdrawal" && variable != "netting")
    {
        return false;
    }

    if (operatorType != "less" && operatorType != "equal" && operatorType != "greater")
    {
        return false;
    }

    if (hours.empty() || *std::min_element(hours.begin(), hours.end()) < 1
        || *std::max_element(hours.begin(), hours.end()) > 168)
    {
        return false;
    }

    return true;
}
} // namespace Antares::Data::ShortTermStorage
