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

#include "renewable-cluster.h"
#include "../item/renewable-cluster-item.h"
#include "../item/group.h"
#include "../component.h"
#include <map>
#include <list>

using namespace Yuni;

namespace Antares
{
namespace Component
{
namespace HTMLListbox
{
namespace Datasource
{

RenewableClustersByOrder::RenewableClustersByOrder(HTMLListbox::Component& parent) :
ClustersByOrder(parent)
{}

RenewableClustersByOrder::~RenewableClustersByOrder()
{}

void GetRenewableClusterMap(Data::Area* area, RenewableClusterMap& l, const wxString& search)
{
    wxString grp;

    const Data::RenewableClusterList::iterator end = area->renewable.list.end();
    for (Data::RenewableClusterList::iterator i = area->renewable.list.begin(); i != end; ++i)
    {
        Data::RenewableCluster* cluster = i->second.get();

        if (search.empty())
        {
            grp = wxStringFromUTF8(cluster->group());
            grp.MakeLower();
            l[grp].push_back(cluster);
        }
    }
}

void RenewableClustersByOrder::refresh(const wxString& search)
{
    pParent.clear();

    if (pArea)
    {
        RenewableClusterMap l;
        GetRenewableClusterMap(pArea, l, search);
        if (!l.empty())
        {
            RenewableClusterMap::iterator end = l.end();
            for (RenewableClusterMap::iterator group_it = l.begin(); group_it != end; ++group_it)
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



RenewableClustersByAlphaOrder::RenewableClustersByAlphaOrder(HTMLListbox::Component& parent) :
    RenewableClustersByOrder(parent)
{}

RenewableClustersByAlphaOrder::~RenewableClustersByAlphaOrder()
{}

void RenewableClustersByAlphaOrder::refreshClustersInGroup(RenewableClusterList& clusterList)
{
    // Added the area as a result
    RenewableClusterList::iterator jend = clusterList.end();
    clusterList.sort(SortAlphaOrder());
    for (RenewableClusterList::iterator j = clusterList.begin(); j != jend; ++j)
        pParent.add(new Antares::Component::HTMLListbox::Item::RenewableClusterItem(*j));
}



RenewableClustersByAlphaReverseOrder::RenewableClustersByAlphaReverseOrder(HTMLListbox::Component& parent) :
    RenewableClustersByOrder(parent)
{}

RenewableClustersByAlphaReverseOrder::~RenewableClustersByAlphaReverseOrder()
{}

void RenewableClustersByAlphaReverseOrder::refreshClustersInGroup(RenewableClusterList& clusterList)
{
    // Added the area as a result
    RenewableClusterList::iterator jend = clusterList.end();
    clusterList.sort(SortAlphaReverseOrder());
    for (RenewableClusterList::iterator j = clusterList.begin(); j != jend; ++j)
        pParent.add(new Antares::Component::HTMLListbox::Item::RenewableClusterItem(*j));
}

} // namespace Datasource
} // namespace HTMLListbox
} // namespace Component
} // namespace Antares
