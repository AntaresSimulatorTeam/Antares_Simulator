// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0


#include "../item/group.h"
#include "../component.h"
#include "thermal-cluster-order.h"
#include "../item/thermal-cluster-item.h"

using namespace Yuni;

namespace Antares::Component::HTMLListbox::Datasource
{
ThermalClustersByOrder::ThermalClustersByOrder(HTMLListbox::Component& parent):
    ClustersByOrder(parent)
{
}

ThermalClustersByOrder::~ThermalClustersByOrder()
{
}

void GetThermalClusterMap(Data::Area* area, ThermalClusterMap& l, const wxString& search)
{
    wxString grp;

    for (auto& cluster: area->thermal.list.all())
    {
        if (search.empty())
        {
            grp = wxStringFromUTF8(cluster->getGroup());
            grp.MakeLower();
            l[grp].push_back(cluster.get());
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
        for (ThermalClusterList::iterator j = groupClusterList.begin(); j != groupClusterList.end();
             ++j)
        {
            size_to_return++;
        }
    }
    return size_to_return;
}

void ThermalClustersByOrder::reorderItemsList(const wxString& search)
{
    if (pArea)
    {
        ThermalClusterMap l;
        GetThermalClusterMap(pArea, l, search);

        // In case the cluster group is new to the item list, we resize the list
        int nombreItems = sizeThermalClusterMap(l);
        pParent.resizeTo(nombreItems);

        int index_item = 0;
        for (ThermalClusterMap::iterator group_it = l.begin(); group_it != l.end(); ++group_it)
        {
            wxString groupName = group_it->first;
            IItem::Ptr groupItem;
            ThermalClusterList& groupClusterList = group_it->second;

            if (groups_to_items_.find(groupName) != groups_to_items_.end())
            {
                groupItem = groups_to_items_[groupName];
            }
            else
            {
                wxString groupTitle = groupNameToGroupTitle(pArea, groupName);
                groupItem = std::make_shared<Group>(groupTitle);
            }
            pParent.setElement(groupItem, index_item);
            index_item++;

            sortClustersInGroup(groupClusterList);

            for (ThermalClusterList::iterator j = groupClusterList.begin();
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

void ThermalClustersByOrder::rebuildItemsList(const wxString& search)
{
    pParent.clear();
    pClustersToItems.clear();
    groups_to_items_.clear();

    if (pArea)
    {
        ThermalClusterMap l;
        GetThermalClusterMap(pArea, l, search);
        for (ThermalClusterMap::iterator group_it = l.begin(); group_it != l.end(); ++group_it)
        {
            wxString groupName = group_it->first;
            wxString groupTitle = groupNameToGroupTitle(pArea, groupName);
            ThermalClusterList& groupClusterList = group_it->second;

            // Refreshing the group
            auto groupItem = std::make_shared<Group>(groupTitle);
            pParent.add(groupItem);
            // Mapping group name to cluster item for possible further usage
            groups_to_items_[groupName] = groupItem;

            // Refreshing all clusters of the group
            sortClustersInGroup(groupClusterList);

            for (ThermalClusterList::iterator j = groupClusterList.begin();
                 j != groupClusterList.end();
                 ++j)
            {
                auto clusterItem = std::make_shared<ThermalClusterItem>(*j);
                pParent.add(clusterItem);
                // Mapping real cluster to cluster item for possible further usage
                pClustersToItems[*j] = clusterItem;
            }
        }
    }
    pParent.forceReload();
}

// -------------------
// Alphabetic order
// -------------------
ThermalClustersByAlphaOrder::ThermalClustersByAlphaOrder(HTMLListbox::Component& parent):
    ThermalClustersByOrder(parent)
{
}

ThermalClustersByAlphaOrder::~ThermalClustersByAlphaOrder()
{
}

void ThermalClustersByAlphaOrder::sortClustersInGroup(ThermalClusterList& clusterList)
{
    clusterList.sort(SortAlphaOrder());
}

// --------------------------
// Alphabetic reverse order
// --------------------------
ThermalClustersByAlphaReverseOrder::ThermalClustersByAlphaReverseOrder(
  HTMLListbox::Component& parent):
    ThermalClustersByOrder(parent)
{
}

ThermalClustersByAlphaReverseOrder::~ThermalClustersByAlphaReverseOrder()
{
}

void ThermalClustersByAlphaReverseOrder::sortClustersInGroup(ThermalClusterList& clusterList)
{
    clusterList.sort(SortAlphaReverseOrder());
}

} // namespace Antares::Component::HTMLListbox::Datasource
