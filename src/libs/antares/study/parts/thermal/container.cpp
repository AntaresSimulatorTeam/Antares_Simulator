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

#include <yuni/yuni.h>
#include "../../study.h"
#include "../../memory-usage.h"
#include "container.h"
#include "../../../logs.h"

using namespace Yuni;
using namespace Antares;

namespace Antares
{
namespace Data
{
PartThermal::PartThermal() :
 unsuppliedEnergyCost(0.),
 spilledEnergyCost(0.),
 list(),
 mustrunList(),
 clusters(nullptr),
 clusterCount((uint)-1)
{
}

bool PartThermal::invalidate(bool reload) const
{
    bool ret = true;
    ret = list.invalidate(reload) && ret;
    ret = mustrunList.invalidate(reload) && ret;
    return ret;
}

void PartThermal::markAsModified() const
{
    list.markAsModified();
    mustrunList.markAsModified();
}

void PartThermal::estimateMemoryUsage(StudyMemoryUsage& u) const
{
    u.requiredMemoryForInput += sizeof(PartThermal);
    list.estimateMemoryUsage(u);
}

PartThermal::~PartThermal()
{
    if (clusterCount)
        delete[] clusters;
}

void PartThermal::prepareAreaWideIndexes()
{
    // Copy the list with all thermal clusters
    // And init the areaWideIndex (unique index for a given area)
    clusterCount = list.size();
    delete[] clusters;
    if (!clusterCount)
    {
        clusters = nullptr;
        return;
    }

    typedef ThermalCluster* ThermalClusterPointer;
    clusters = new ThermalClusterPointer[clusterCount];

    auto end = list.end();
    uint idx = 0;
    for (auto i = list.begin(); i != end; ++i)
    {
        ThermalCluster* t = i->second.get();
        t->areaWideIndex = idx;
        clusters[idx] = t;
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
        auto end = list.end();
        for (auto i = list.begin(); i != end; ++i)
        {
            if ((i->second)->mustrun)
            {
                // Detaching the thermal cluster from the main list...
                ThermalCluster* cluster = list.detach(i);
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
        }
    } while (mustContinue);

    // if some thermal cluster has been moved, we must rebuild all the indexes
    if (count)
    {
        list.rebuildIndex();
        mustrunList.rebuildIndex();
    }

    return count;
}

uint PartThermal::removeDisabledClusters()
{
    // nothing to do if there is no cluster available
    if (list.empty())
        return 0;

    std::vector<ClusterName> disabledClusters;

    for (auto& it : list)
    {
        if (!it.second->enabled)
            disabledClusters.push_back(it.first);
    }

    for (const auto& cluster : disabledClusters)
        list.remove(cluster);

    return disabledClusters.size();
}

void PartThermal::reset()
{
    unsuppliedEnergyCost = 0.;
    spilledEnergyCost = 0.;

    mustrunList.clear();
    list.clear();

    // just in case
    clusterCount = 0;
    delete[] clusters;
}

} // namespace Data
} // namespace Antares
