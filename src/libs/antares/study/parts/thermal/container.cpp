// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/study/parts/thermal/container.h"

#include <algorithm>
#include <functional>

#include <yuni/yuni.h>

#include <antares/logs/logs.h>
#include "antares/study/study.h"

using namespace Yuni;
using namespace Antares;

namespace Antares::Data
{

PartThermal::PartThermal():
    unsuppliedEnergyCost(0.),
    spilledEnergyCost(0.)
{
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

} // namespace Antares::Data
