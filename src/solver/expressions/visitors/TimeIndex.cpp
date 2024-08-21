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

#include <antares/solver/expressions/visitors/TimeIndex.h>

namespace Antares::Solver::Visitors
{
TimeIndex::TimeIndex(bool time_varying, bool scenario_varying):
    time_varying_(time_varying),
    scenario_varying_(scenario_varying)
{
}

bool TimeIndex::IsTimeVarying() const
{
    return time_varying_;
}

bool TimeIndex::IsScenarioVarying() const
{
    return scenario_varying_;
}

TimeIndex TimeIndex::Connect(const TimeIndex& other) const
{
    bool is_time_varying = time_varying_ || other.time_varying_;
    bool is_scenario_varying = scenario_varying_ || other.scenario_varying_;
    return TimeIndex(is_time_varying, is_scenario_varying);
}
} // namespace Antares::Solver::Visitors
