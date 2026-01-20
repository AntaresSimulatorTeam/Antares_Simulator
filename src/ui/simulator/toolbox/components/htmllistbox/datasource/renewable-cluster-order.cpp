// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0


#include "../item/group.h"
#include "../component.h"
#include "renewable-cluster-order.h"
#include "../item/renewable-cluster-item.h"

using namespace Yuni;

namespace Antares::Component::HTMLListbox::Datasource
{
RenewableClustersByOrder::RenewableClustersByOrder(HTMLListbox::Component& parent):
    ClustersByOrder(parent)
{
}

RenewableClustersByOrder::~RenewableClustersByOrder()
{
}

void GetRenewableClusterMap(Data::Area* area, RenewableClusterMap& l, const wxString& search)
{
    wxString grp;

    for (auto& cluster: area->renewable.list.all())
    {
        if (search.empty())
        {
            grp = wxStringFromUTF8(cluster->getGroup());
            grp.MakeLower();
            l[grp].push_back(cluster.get());
        }
    }
}

int sizeRenewableClusterMap(RenewableClusterMap& l)
{
    int size_to_return = 0;
    for (RenewableClusterMap::iterator group_it = l.begin(); group_it != l.end(); ++group_it)
    {
        size_to_return++;

        RenewableClusterList& groupClusterList = group_it->second;
        for (RenewableClusterList::iterator j = groupClusterList.begin();
             j != groupClusterList.end();
             ++j)
        {
            size_to_return++;
        }
    }
    return size_to_return;
}

void RenewableClustersByOrder::reorderItemsList(const wxString& search)
{
    if (pArea)
    {
        RenewableClusterMap l;
        GetRenewableClusterMap(pArea, l, search);

        // In case the cluster group is new to the item list, we resize the list
        int nombreItems = sizeRenewableClusterMap(l);
        pParent.resizeTo(nombreItems);

        int index_item = 0;
        for (RenewableClusterMap::iterator group_it = l.begin(); group_it != l.end(); ++group_it)
        {
            wxString groupName = group_it->first;
            IItem::Ptr groupItem;
            RenewableClusterList& groupClusterList = group_it->second;

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

            for (RenewableClusterList::iterator j = groupClusterList.begin();
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

void RenewableClustersByOrder::rebuildItemsList(const wxString& search)
{
    pParent.clear();
    pClustersToItems.clear();
    groups_to_items_.clear();

    if (pArea)
    {
        RenewableClusterMap l;
        GetRenewableClusterMap(pArea, l, search);
        for (RenewableClusterMap::iterator group_it = l.begin(); group_it != l.end(); ++group_it)
        {
            wxString groupName = group_it->first;
            wxString groupTitle = groupNameToGroupTitle(pArea, groupName);
            RenewableClusterList& groupClusterList = group_it->second;

            // Refreshing the group
            auto groupItem = std::make_shared<Group>(groupTitle);
            pParent.add(groupItem);
            // Mapping group name to cluster item for possible further usage
            groups_to_items_[groupName] = groupItem;

            // Refreshing all clusters of the group
            sortClustersInGroup(groupClusterList);

            for (RenewableClusterList::iterator j = groupClusterList.begin();
                 j != groupClusterList.end();
                 ++j)
            {
                auto clusterItem = std::make_shared<RenewableClusterItem>(*j);
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
RenewableClustersByAlphaOrder::RenewableClustersByAlphaOrder(HTMLListbox::Component& parent):
    RenewableClustersByOrder(parent)
{
}

RenewableClustersByAlphaOrder::~RenewableClustersByAlphaOrder()
{
}

void RenewableClustersByAlphaOrder::sortClustersInGroup(RenewableClusterList& clusterList)
{
    clusterList.sort(SortAlphaOrder());
}

// --------------------------
// Alphabetic reverse order
// --------------------------
RenewableClustersByAlphaReverseOrder::RenewableClustersByAlphaReverseOrder(
  HTMLListbox::Component& parent):
    RenewableClustersByOrder(parent)
{
}

RenewableClustersByAlphaReverseOrder::~RenewableClustersByAlphaReverseOrder()
{
}

void RenewableClustersByAlphaReverseOrder::sortClustersInGroup(RenewableClusterList& clusterList)
{
    clusterList.sort(SortAlphaReverseOrder());
}

} // namespace Antares::Component::HTMLListbox::Datasource
