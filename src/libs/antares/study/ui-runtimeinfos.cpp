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

#include "antares/study/ui-runtimeinfos.h"

#include <yuni/yuni.h>

#include "antares/study/study.h"

using namespace Yuni;

namespace Antares::Data
{
UIRuntimeInfo::UIRuntimeInfo(Study& study):
    pStudy(study)
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
    pLinkCount = 0;
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
                {
                    set.insert(i->second);
                }
            }

            for (auto& cluster: area->thermal.list.each_enabled())
            {
                pClusters.push_back(cluster.get());
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
                ++pLinkCount;
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
        const auto end = pStudy.bindingConstraints.end();
        auto i = pStudy.bindingConstraints.begin();
        for (; i != end; ++i)
        {
            auto bc = *i;
            orderedConstraint.insert(bc);
        }
    }
    {
        const auto end = orderedConstraint.end();
        auto i = orderedConstraint.begin();
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

uint UIRuntimeInfo::countItems(BindingConstraint::Operator op, BindingConstraint::Type type)
{
    ByOperatorAndType::const_iterator i = byOperator.find(op);
    if (i != byOperator.end())
    {
        VectorByType::const_iterator j = i->second.find(type);
        if (j != i->second.end())
        {
            return (uint)j->second.size();
        }
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
        {
            count++;
        }
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
        {
            count++;
        }
    }
    return count;
}

} // namespace Antares::Data
