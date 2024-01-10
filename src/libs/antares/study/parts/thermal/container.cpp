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
** There are special exceptions to the terms and conditions of the
** license as they are applied to this software. View the full text of
** the exceptions in file COPYING.txt in the directory of this software
** distribution
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
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
    ret = mustrunList.forceReload(reload) && ret;
    return ret;
}

void PartThermal::markAsModified() const
{
    list.markAsModified();
    mustrunList.markAsModified();
}

PartThermal::~PartThermal()
{
}

void PartThermal::prepareAreaWideIndexes()
{
    // Copy the list with all thermal clusters
    // And init the areaWideIndex (unique index for a given area)
    if (list.empty())
    {
        clusters.clear();
        return;
    }

    clusters.assign(list.size(), nullptr);

    uint idx = 0;
    for (const auto& cluster : list)
    {
        cluster->areaWideIndex = idx;
        clusters[idx] = cluster.get();
        ++idx;
    }
}

uint PartThermal::prepareClustersInMustRunMode()
{
    // nothing to do if there is no cluster available
    if (list.empty())
        return 0;

    // the number of clusters in 'must-run' mode
    uint count = 0;
    bool mustContinue;
    do
    {
        mustContinue = false;
        for (auto cluster : list)
        {
            if (!cluster->mustrun)
                continue;

            // Detaching the thermal cluster from the main list...
            list.remove(cluster->id());
            if (!cluster->enabled)
                continue;
            // ...and attaching it into the second list
            if (!mustrunList.add(cluster))
            {
                logs.error() << "Impossible to prepare the thermal cluster in 'must-run' mode: "
                    << cluster->parentArea->name << "::" << cluster->name();
            }
            else
            {
                ++count;
                logs.info() << "enabling 'must-run' mode for the cluster  "
                    << cluster->parentArea->name << "::" << cluster->name();
            }

            // the iterator has been invalidated, loop again
            mustContinue = true;
            break;
        }
    } while (mustContinue);

    return count;
}

void PartThermal::reset()
{
    unsuppliedEnergyCost = 0.;
    spilledEnergyCost = 0.;

    mustrunList.clear();
    list.clear();
    clusters.clear();
}

bool PartThermal::hasForcedTimeseriesGeneration() const
{
    using Behavior = LocalTSGenerationBehavior;
    return std::any_of(list.begin(), list.end(), [](const ThermalClusterList::SharedPtr& cluster) {
        return cluster->tsGenBehavior == Behavior::forceGen;
    });
}

bool PartThermal::hasForcedNoTimeseriesGeneration() const
{
    using Behavior = LocalTSGenerationBehavior;
    return std::any_of(list.begin(), list.end(), [](const ThermalClusterList::SharedPtr& cluster) {
        return cluster->tsGenBehavior == Behavior::forceNoGen;
    });
}

void PartThermal::checkAndCorrectAvailability()
{
    std::for_each(
      clusters.begin(), clusters.end(), std::mem_fn(&ThermalCluster::checkAndCorrectAvailability));
}

} // namespace Data
} // namespace Antares
