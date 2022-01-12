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
#include "study.h"
#include "ui-runtimeinfos.h"

using namespace Yuni;

namespace Antares
{
namespace Data
{
UIRuntimeInfo::UIRuntimeInfo(Study& study) : pStudy(study)
{
    reloadAll();
}

void UIRuntimeInfo::reloadAll()
{
    reload();
    reloadBindingConstraints();
}

void UIRuntimeInfo::reload()
{
    // Reseting all variables
    pLink.clear();
    pClusters.clear();
    pClusterCount = 0;
    orderedAreasAndLinks.clear();

    // Building an ordered list of areas then links
    {
        const Area::Map::iterator end = pStudy.areas.end();
        for (Area::Map::iterator i = pStudy.areas.begin(); i != end; ++i)
        {
            // Reference to the area
            Area* area = i->second;
            assert(area && "Invalid area");

            // Ordered by its color
            assert(area->ui && "Invalid area UI");
            (area->ui)->rebuildCache();

            // Reference to the set
            // This variable is not within the condition that follows this expression
            // to create an empty set even if no link is present.
            AreaLink::Set& set = orderedAreasAndLinks[area];

            if (!area->links.empty())
            {
                const AreaLink::Map::iterator end = area->links.end();
                for (AreaLink::Map::iterator i = area->links.begin(); i != end; ++i)
                    set.insert(i->second);
            }

            for (uint j = 0; j < area->thermal.clusterCount(); ++j)
            {
                ThermalCluster* cluster = area->thermal.clusters[j];
                pClusters.push_back(cluster);
            }
        }
    }

    pClusterCount = (uint)pClusters.size();

    // Sorting the links between the areas as well
    {
        Area::LinkMap::const_iterator end = orderedAreasAndLinks.end();
        Area::LinkMap::const_iterator i = orderedAreasAndLinks.begin();
        for (; i != end; ++i)
        {
            AreaLink::Set::const_iterator jend = i->second.end();
            AreaLink::Set::const_iterator j = i->second.begin();
            for (; j != jend; ++j)
            {
                pLink.push_back(*j);
            }
        }
        std::sort(pLink.begin(), pLink.end(), CompareLinkName());
    }
}

void UIRuntimeInfo::reloadBindingConstraints()
{
    orderedConstraint.clear();
    pConstraint.clear();
    byOperator.clear();

    {
        const BindConstList::iterator end = pStudy.bindingConstraints.end();
        BindConstList::iterator i = pStudy.bindingConstraints.begin();
        for (; i != end; ++i)
            orderedConstraint.insert(*i);
    }
    {
        const BindingConstraint::Set::const_iterator end = orderedConstraint.end();
        BindingConstraint::Set::const_iterator i = orderedConstraint.begin();
        for (; i != end; ++i)
        {
            pConstraint.push_back(*i);
            switch ((*i)->operatorType())
            {
            case BindingConstraint::opBoth:
            {
                byOperator[BindingConstraint::opLess][(*i)->type()].push_back(*i);
                byOperator[BindingConstraint::opGreater][(*i)->type()].push_back(*i);
                break;
            }
            default:
            {
                byOperator[(*i)->operatorType()][(*i)->type()].push_back(*i);
                break;
            }
            }
        }
    }
}

uint64 UIRuntimeInfo::memoryUsage() const
{
    return sizeof(UIRuntimeInfo) + sizeof(AreaLink*) * pLink.size();
}

uint UIRuntimeInfo::countItems(BindingConstraint::Operator op, BindingConstraint::Type type)
{
    ByOperatorAndType::const_iterator i = byOperator.find(op);
    if (i != byOperator.end())
    {
        VectorByType::const_iterator j = i->second.find(type);
        if (j != i->second.end())
            return (uint)j->second.size();
    }
    return 0;
}

uint UIRuntimeInfo::visibleClustersCount(uint layerID)
{
    int count = 0;
    auto cEnd = pClusters.end();
    for (auto cluster = pClusters.begin(); cluster != cEnd; cluster++)
    {
        if ((*cluster)->isVisibleOnLayer(layerID))
            count++;
    }
    return count;
}

uint UIRuntimeInfo::visibleLinksCount(uint layerID)
{
    int count = 0;
    auto lEnd = pLink.end();
    for (auto link = pLink.begin(); link != lEnd; link++)
    {
        if ((*link)->isVisibleOnLayer(layerID))
            count++;
    }
    return count;
}

} // namespace Data
} // namespace Antares
