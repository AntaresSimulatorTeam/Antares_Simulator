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
#pragma once
#include <antares/solver/expressions/nodes/Node.h>

namespace Antares::Solver::Nodes
{
enum class TimeType
{
    CONSTANT,
    TIME_DEPENDANT_ONLY,
    SCENARIZED_ONLY,
    TIME_DEPENDANT_AND_SCENARIZED
};

// TODO better name?
class TimeIndex
{
public:
    TimeIndex() = default;

    constexpr explicit TimeIndex(TimeType timeType):
        timeType_(timeType)
    {
    }

    [[nodiscard]] constexpr TimeType getTimeType() const
    {
        return timeType_;
    }

private:
    TimeType timeType_ = TimeType::CONSTANT;
};
} // namespace Antares::Solver::Nodes
