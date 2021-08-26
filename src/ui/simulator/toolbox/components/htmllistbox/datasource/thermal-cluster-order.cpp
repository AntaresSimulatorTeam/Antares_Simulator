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
#include "thermal-cluster-order.h"
#include "../item/thermal-cluster-item.h"

using namespace Yuni;

namespace Antares
{
namespace Component
{
namespace HTMLListbox
{
namespace Datasource
{

// gp : to remove ?
// using namespace Antares::Component::HTMLListbox::Item;

ThermalClustersByOrder::ThermalClustersByOrder(HTMLListbox::Component& parent) :
    ClustersByOrder(parent)
{}

ThermalClustersByOrder::~ThermalClustersByOrder()
{}

void GetThermalClusterMap(Data::Area* area, ThermalClusterMap& l, const wxString& search)
{
    wxString grp;

    const Data::ThermalClusterList::iterator end = area->thermal.list.end();
    for (Data::ThermalClusterList::iterator i = area->thermal.list.begin(); i != end; ++i)
    {
        Data::ThermalCluster* cluster = i->second.get();

        if (search.empty())
        {
            grp = wxStringFromUTF8(cluster->group());
            grp.MakeLower();
            l[grp].push_back(cluster);
        }
    }
}

int sizeThermalClusterMap(ThermalClusterMap& l)
{
    int size_to_return = 0;
    for (ThermalClusterMap::iterator group_it = l.begin(); group_it != l.end(); ++group_it)
    {
        size_to_return++;

        ThermalClusterList& groupClusterList = group_it->second;
        for (ThermalClusterList::iterator j = groupClusterList.begin(); j != groupClusterList.end(); ++j)
            size_to_return++;
    }
    return size_to_return;
}

void ThermalClustersByOrder::reordering_items_list(const wxString& search)
{
    if (pArea)
    {
        ThermalClusterMap l;
        GetThermalClusterMap(pArea, l, search);

        // In case the cluster group is new to the item list, we resize the list 
        int nombreItems = sizeThermalClusterMap(l);
        pParent.resizeTo(nombreItems);

        if (!l.empty())
        {
            int index_item = 0;
            for (ThermalClusterMap::iterator group_it = l.begin(); group_it != l.end(); ++group_it)
            {
                wxString groupName = group_it->first;
                IItem* groupItem;
                ThermalClusterList& groupClusterList = group_it->second;

                if (groups_to_items_.find(groupName) != groups_to_items_.end())
                    groupItem = groups_to_items_[groupName];
                else
                {
                    wxString groupTitle = groupNameToGroupTitle(pArea, groupName);
                    groupItem = new Group(groupTitle);
                }
                pParent.setElement(groupItem, index_item);
                index_item++;

                sortClustersInGroup(groupClusterList);

                for (ThermalClusterList::iterator j = groupClusterList.begin(); j != groupClusterList.end(); ++j)
                {
                    ClusterItem* clusterItem = clusters_to_items_[*j];
                    pParent.setElement(clusterItem, index_item);
                    index_item++;
                }
            }
        }
    }
}

void ThermalClustersByOrder::rebuilding_items_list(const wxString& search)
{
    pParent.clear();
    clusters_to_items_.clear();
    groups_to_items_.clear();

    if (pArea)
    {
        ThermalClusterMap l;
        GetThermalClusterMap(pArea, l, search);
        if (!l.empty())
        {
            for (ThermalClusterMap::iterator group_it = l.begin(); group_it != l.end(); ++group_it)
            {
                wxString groupName = group_it->first;
                wxString groupTitle = groupNameToGroupTitle(pArea, groupName);
                ThermalClusterList& groupClusterList = group_it->second;

                // Refreshing the group
                IItem* groupItem = new Group(groupTitle);
                pParent.add(groupItem);
                // Mapping group name to cluster item for possible further usage
                groups_to_items_[groupName] = groupItem;

                // Refreshing all clusters of the group
                sortClustersInGroup(groupClusterList);

                for (ThermalClusterList::iterator j = groupClusterList.begin(); j != groupClusterList.end(); ++j)
                {
                    ClusterItem* clusterItem = new ThermalClusterItem(*j);
                    pParent.add(clusterItem);
                    // Mapping real cluster to cluster item for possible further usage
                    clusters_to_items_[*j] = clusterItem;
                }
            }
        }
    }
    pParent.invalidate();
}

// -------------------
// Alphabetic order
// -------------------
ThermalClustersByAlphaOrder::ThermalClustersByAlphaOrder(HTMLListbox::Component& parent) : 
    ThermalClustersByOrder(parent)
{}

ThermalClustersByAlphaOrder::~ThermalClustersByAlphaOrder()
{}

void ThermalClustersByAlphaOrder::sortClustersInGroup(ThermalClusterList& clusterList)
{
    clusterList.sort(SortAlphaOrder());
}


// --------------------------
// Alphabetic reverse order
// --------------------------
ThermalClustersByAlphaReverseOrder::ThermalClustersByAlphaReverseOrder(HTMLListbox::Component& parent) :
    ThermalClustersByOrder(parent)
{}

ThermalClustersByAlphaReverseOrder::~ThermalClustersByAlphaReverseOrder()
{}

void ThermalClustersByAlphaReverseOrder::sortClustersInGroup(ThermalClusterList& clusterList)
{
    clusterList.sort(SortAlphaReverseOrder());
}

} // namespace Datasource
} // namespace HTMLListbox
} // namespace Component
} // namespace Antares
