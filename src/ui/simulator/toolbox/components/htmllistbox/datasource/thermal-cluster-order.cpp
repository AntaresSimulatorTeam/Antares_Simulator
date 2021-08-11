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
#include "../item/thermal-cluster-item.h"
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
    pParent.clear();

    if (pArea)
    {
        ThermalClusterMap l;
        GetThermalClusterMap(pArea, l, search);
        if (!l.empty())
        {
            ThermalClusterMap::iterator end = l.end();
            for (ThermalClusterMap::iterator group_it = l.begin(); group_it != end; ++group_it)
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
                pParent.add(new Antares::Component::HTMLListbox::Item::Group(s));
                
                // Refreshing all clusters of the group
                refreshClustersInGroup(group_it->second);
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

void ThermalClustersByAlphaOrder::refreshClustersInGroup(ThermalClusterList & clusterList)
{
    // Added the area as a result
    ThermalClusterList::iterator jend = clusterList.end();
    clusterList.sort(SortAlphaOrder());
    for (ThermalClusterList::iterator j = clusterList.begin(); j != jend; ++j)
        pParent.add(new Antares::Component::HTMLListbox::Item::ThermalClusterItem(*j));
}



ThermalClustersByAlphaReverseOrder::ThermalClustersByAlphaReverseOrder(HTMLListbox::Component& parent) :
    ThermalClustersByOrder(parent)
{}

ThermalClustersByAlphaReverseOrder::~ThermalClustersByAlphaReverseOrder()
{}

void ThermalClustersByAlphaReverseOrder::refreshClustersInGroup(ThermalClusterList& clusterList)
{
    // Added the area as a result
    ThermalClusterList::iterator jend = clusterList.end();
    clusterList.sort(SortAlphaReverseOrder());
    for (ThermalClusterList::iterator j = clusterList.begin(); j != jend; ++j)
        pParent.add(new Antares::Component::HTMLListbox::Item::ThermalClusterItem(*j));
}

} // namespace Datasource
} // namespace HTMLListbox
} // namespace Component
} // namespace Antares
