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

#include "thermal-cluster-order.h"
#include "../item/group.h"
#include "../component.h"

using namespace Yuni;

namespace Antares
{
namespace Component
{
namespace HTMLListbox
{
namespace Datasource
{

using namespace Antares::Component::HTMLListbox::Item;

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

void ThermalClustersByOrder::refresh(const wxString& search)
{
    if (hasGroupChanged())
    {
        refresh_when_cluster_group_changed(search);
        hasGroupChanged(false);
    }        
    else
        refresh_by_building_item_list(search);
}

void ThermalClustersByOrder::refresh_when_cluster_group_changed(const wxString& search)
{
    // pParent.clear();

    if (pArea)
    {
        ThermalClusterMap l;
        GetThermalClusterMap(pArea, l, search);
        if (!l.empty())
        {
            int index_item = 0;
            // std::vector<IItem> items_tmp;
            for (ThermalClusterMap::iterator group_it = l.begin(); group_it != l.end(); ++group_it)
            {
                if (groups_to_items_.find(group_it->first) != groups_to_items_.end())
                {
                    auto groupItem = groups_to_items_[group_it->first];
                    pParent.setElement(groupItem, index_item);
                }
                else
                {
                    // Group title
                    wxString s;
                    s << wxStringFromUTF8(pArea->name);
                    if (s.size() > 43)
                    {
                        s.resize(40);
                        s += wxT("...");
                    }

                    if (group_it->first.empty())
                        s << wxT(" / <i>* no group *</i>");
                    else
                        s << wxT(" / ") << group_it->first;

                    auto groupItem = new Group(s);
                    pParent.add(groupItem);

                }

                index_item++;

                ThermalClusterList& clusterList = group_it->second;

                sortClustersInGroup(clusterList);

                for (ThermalClusterList::iterator j = clusterList.begin(); j != clusterList.end(); ++j)
                {
                    
                    // pParent.setElement(clusters_to_items_[*j].first, index_item);

                    auto thermalClusterItem = clusters_to_items_[*j];
                    if (index_item + 1 > pParent.size())
                        pParent.add(thermalClusterItem);
                    else
                        pParent.setElement(thermalClusterItem, index_item);

                    index_item++;
                }
            }
        }
    }
    pParent.invalidate();
}

void ThermalClustersByOrder::refresh_by_building_item_list(const wxString& search)
{
    pParent.clear();
    clusters_to_items_.clear();

    if (pArea)
    {
        ThermalClusterMap l;
        GetThermalClusterMap(pArea, l, search);
        if (!l.empty())
        {
            // int index_item = 0;
            for (ThermalClusterMap::iterator group_it = l.begin(); group_it != l.end(); ++group_it)
            {
                // Group title
                wxString s;
                s << wxStringFromUTF8(pArea->name);
                if (s.size() > 43)
                {
                    s.resize(40);
                    s += wxT("...");
                }

                if (group_it->first.empty())
                    s << wxT(" / <i>* no group *</i>");
                else
                    s << wxT(" / ") << group_it->first;

                // Refreshing the group
                auto groupItem = new Group(s);
                pParent.add(groupItem);
                groups_to_items_[group_it->first] = groupItem;
                // index_item++;

                // Refreshing all clusters of the group
                ThermalClusterList& clusterList = group_it->second;
                sortClustersInGroup(clusterList);

                for (ThermalClusterList::iterator j = clusterList.begin(); j != clusterList.end(); ++j)
                {
                    auto thermalClusterItem = new ThermalClusterItem(*j);
                    pParent.add(thermalClusterItem);
                    // clusters_to_items_[*j] = std::make_pair(thermalClusterItem, index_item);
                    clusters_to_items_[*j] = thermalClusterItem;
                    // index_item++;
                }
            }
        }
    }
    pParent.invalidate();
}



ThermalClustersByAlphaOrder::ThermalClustersByAlphaOrder(HTMLListbox::Component& parent) : 
    ThermalClustersByOrder(parent)
{}

ThermalClustersByAlphaOrder::~ThermalClustersByAlphaOrder()
{}

void ThermalClustersByAlphaOrder::sortClustersInGroup(ThermalClusterList& clusterList)
{
    clusterList.sort(SortAlphaOrder());
}

/*
void ThermalClustersByAlphaOrder::refreshClustersInGroup(ThermalClusterList & clusterList)
{
    // Added the area as a result
    ThermalClusterList::iterator jend = clusterList.end();
    clusterList.sort(SortAlphaOrder());
    for (ThermalClusterList::iterator j = clusterList.begin(); j != jend; ++j)
    {
        auto thermalClusterItem = new Antares::Component::HTMLListbox::Item::ThermalClusterItem(*j);
        pParent.add(thermalClusterItem);
        clusters_to_items_[*j] = thermalClusterItem;
    }
}
*/



ThermalClustersByAlphaReverseOrder::ThermalClustersByAlphaReverseOrder(HTMLListbox::Component& parent) :
    ThermalClustersByOrder(parent)
{}

ThermalClustersByAlphaReverseOrder::~ThermalClustersByAlphaReverseOrder()
{}

void ThermalClustersByAlphaReverseOrder::sortClustersInGroup(ThermalClusterList& clusterList)
{
    clusterList.sort(SortAlphaReverseOrder());
}

/*
void ThermalClustersByAlphaReverseOrder::refreshClustersInGroup(ThermalClusterList& clusterList)
{
    // Added the area as a result
    ThermalClusterList::iterator jend = clusterList.end();
    clusterList.sort(SortAlphaReverseOrder());
    for (ThermalClusterList::iterator j = clusterList.begin(); j != jend; ++j)
        pParent.add(new Antares::Component::HTMLListbox::Item::ThermalClusterItem(*j));
}
*/

} // namespace Datasource
} // namespace HTMLListbox
} // namespace Component
} // namespace Antares
