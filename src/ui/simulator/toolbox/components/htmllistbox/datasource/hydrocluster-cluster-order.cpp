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

#include "../item/group.h"
#include "../component.h"
#include "hydrocluster-cluster-order.h"
#include "../item/hydrocluster-cluster-item.h"

using namespace Yuni;

namespace Antares
{
namespace Component
{
namespace HTMLListbox
{
namespace Datasource
{
HydroclusterClustersByOrder::HydroclusterClustersByOrder(HTMLListbox::Component& parent) :
 ClustersByOrder(parent)
{
}

HydroclusterClustersByOrder::~HydroclusterClustersByOrder()
{
}

void GetHydroclusterClusterMap(Data::Area* area, HydroclusterClusterMap& l, const wxString& search)
{
    wxString grp;

    const Data::HydroclusterClusterList::iterator end = area->renewable.list.end();
    for (Data::HydroclusterClusterList::iterator i = area->renewable.list.begin(); i != end; ++i)
    {
        Data::HydroclusterCluster* cluster = i->second.get();

        if (search.empty())
        {
            grp = wxStringFromUTF8(cluster->group());
            grp.MakeLower();
            l[grp].push_back(cluster);
        }
    }
}

int sizeHydroclusterClusterMap(HydroclusterClusterMap& l)
{
    int size_to_return = 0;
    for (HydroclusterClusterMap::iterator group_it = l.begin(); group_it != l.end(); ++group_it)
    {
        size_to_return++;

        HydroclusterClusterList& groupClusterList = group_it->second;
        for (HydroclusterClusterList::iterator j = groupClusterList.begin();
             j != groupClusterList.end();
             ++j)
            size_to_return++;
    }
    return size_to_return;
}

void HydroclusterClustersByOrder::reorderItemsList(const wxString& search)
{
    if (pArea)
    {
        HydroclusterClusterMap l;
        GetHydroclusterClusterMap(pArea, l, search);

        // In case the cluster group is new to the item list, we resize the list
        int nombreItems = sizeHydroclusterClusterMap(l);
        pParent.resizeTo(nombreItems);

        int index_item = 0;
        for (HydroclusterClusterMap::iterator group_it = l.begin(); group_it != l.end(); ++group_it)
        {
            wxString groupName = group_it->first;
            IItem::Ptr groupItem;
            HydroclusterClusterList& groupClusterList = group_it->second;

            if (groups_to_items_.find(groupName) != groups_to_items_.end())
                groupItem = groups_to_items_[groupName];
            else
            {
                wxString groupTitle = groupNameToGroupTitle(pArea, groupName);
                groupItem = std::make_shared<Group>(groupTitle);
            }
            pParent.setElement(groupItem, index_item);
            index_item++;

            sortClustersInGroup(groupClusterList);

            for (HydroclusterClusterList::iterator j = groupClusterList.begin();
                 j != groupClusterList.end();
                 ++j)
            {
                auto clusterItem = pClustersToItems[*j];
                pParent.setElement(clusterItem, index_item);
                index_item++;
            }
        }
    }
}

void HydroclusterClustersByOrder::rebuildItemsList(const wxString& search)
{
    pParent.clear();
    pClustersToItems.clear();
    groups_to_items_.clear();

    if (pArea)
    {
        HydroclusterClusterMap l;
        GetHydroclusterClusterMap(pArea, l, search);
        for (HydroclusterClusterMap::iterator group_it = l.begin(); group_it != l.end(); ++group_it)
        {
            wxString groupName = group_it->first;
            wxString groupTitle = groupNameToGroupTitle(pArea, groupName);
            HydroclusterClusterList& groupClusterList = group_it->second;

            // Refreshing the group
            auto groupItem = std::make_shared<Group>(groupTitle);
            pParent.add(groupItem);
            // Mapping group name to cluster item for possible further usage
            groups_to_items_[groupName] = groupItem;

            // Refreshing all clusters of the group
            sortClustersInGroup(groupClusterList);

            for (HydroclusterClusterList::iterator j = groupClusterList.begin();
                 j != groupClusterList.end();
                 ++j)
            {
                auto clusterItem = std::make_shared<HydroclusterClusterItem>(*j);
                pParent.add(clusterItem);
                // Mapping real cluster to cluster item for possible further usage
                pClustersToItems[*j] = clusterItem;
            }
        }
    }
    pParent.invalidate();
}

// -------------------
// Alphabetic order
// -------------------
HydroclusterClustersByAlphaOrder::HydroclusterClustersByAlphaOrder(HTMLListbox::Component& parent) :
 HydroclusterClustersByOrder(parent)
{
}

HydroclusterClustersByAlphaOrder::~HydroclusterClustersByAlphaOrder()
{
}

void HydroclusterClustersByAlphaOrder::sortClustersInGroup(HydroclusterClusterList& clusterList)
{
    clusterList.sort(SortAlphaOrder());
}

// --------------------------
// Alphabetic reverse order
// --------------------------
HydroclusterClustersByAlphaReverseOrder::HydroclusterClustersByAlphaReverseOrder(
  HTMLListbox::Component& parent) :
 HydroclusterClustersByOrder(parent)
{
}

HydroclusterClustersByAlphaReverseOrder::~HydroclusterClustersByAlphaReverseOrder()
{
}

void HydroclusterClustersByAlphaReverseOrder::sortClustersInGroup(HydroclusterClusterList& clusterList)
{
    clusterList.sort(SortAlphaReverseOrder());
}

} // namespace Datasource
} // namespace HTMLListbox
} // namespace Component
} // namespace Antares
