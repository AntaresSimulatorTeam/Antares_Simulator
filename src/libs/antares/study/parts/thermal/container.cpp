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

#include "antares/study/parts/thermal/container.h"

#include <algorithm>
#include <functional>

#include <yuni/yuni.h>

#include <antares/logs/logs.h>
#include "antares/study/study.h"

using namespace Yuni;
using namespace Antares;

namespace Antares
{
namespace Data
{

PartThermal::PartThermal():
    unsuppliedEnergyCost(0.),
    spilledEnergyCost(0.)
{
}

bool PartThermal::forceReload(bool reload) const
{
    bool ret = true;
    ret = list.forceReload(reload) && ret;
    return ret;
}

void PartThermal::markAsModified() const
{
    list.markAsModified();
}

void PartThermal::reset()
{
    unsuppliedEnergyCost = 0.;
    spilledEnergyCost = 0.;

    list.clearAll();
}

void PartThermal::resizeAllTimeseriesNumbers(uint n) const
{
    list.resizeAllTimeseriesNumbers(n);
}

bool PartThermal::hasForcedTimeseriesGeneration() const
{
    using Behavior = LocalTSGenerationBehavior;
    return std::ranges::any_of(list.all(),
                               [](const ThermalClusterList::SharedPtr& cluster)
                               { return cluster->tsGenBehavior == Behavior::forceGen; });
}

bool PartThermal::hasForcedNoTimeseriesGeneration() const
{
    using Behavior = LocalTSGenerationBehavior;
    return std::ranges::any_of(list.all(),
                               [](const ThermalClusterList::SharedPtr& cluster)
                               { return cluster->tsGenBehavior == Behavior::forceNoGen; });
}

void PartThermal::checkAndCorrectAvailability() const
{
    std::ranges::for_each(list.each_enabled(), &ThermalCluster::checkAndCorrectAvailability);
}

} // namespace Data
} // namespace Antares
