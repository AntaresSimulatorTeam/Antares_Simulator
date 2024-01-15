/*
** Copyright 2007-2023 RTE
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

#include <yuni/yuni.h>
#include "../../study.h"
#include "container.h"
#include <antares/logs/logs.h>

#include <algorithm>
#include <functional>

using namespace Yuni;
using namespace Antares;

namespace Antares
{
namespace Data
{

PartThermal::PartThermal() : unsuppliedEnergyCost(0.), spilledEnergyCost(0.)
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

void PartThermal::resizeAllTimeseriesNumbers(uint n)
{
    list.resizeAllTimeseriesNumbers(n);
}


bool PartThermal::hasForcedTimeseriesGeneration() const
{
    using Behavior = LocalTSGenerationBehavior;
    return std::ranges::any_of(list.all(), [](const ThermalClusterList::SharedPtr& cluster) {
        return cluster->tsGenBehavior == Behavior::forceGen;
    });
}

bool PartThermal::hasForcedNoTimeseriesGeneration() const
{
    using Behavior = LocalTSGenerationBehavior;
    return std::ranges::any_of(list.all(), [](const ThermalClusterList::SharedPtr& cluster) {
        return cluster->tsGenBehavior == Behavior::forceNoGen;
    });
}

void PartThermal::checkAndCorrectAvailability()
{
    std::ranges::for_each(list.each_enabled(), &ThermalCluster::checkAndCorrectAvailability);
}

} // namespace Data
} // namespace Antares
