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
// #include "../../../../application/study.h"
#include "../item/renewable-cluster-item.h"
#include "../item/group.h"
#include "../component.h"
#include <map>
#include <list>
// #include "../../../../application/main.h"

using namespace Yuni;

namespace Antares
{
namespace Component
{
namespace HTMLListbox
{
namespace Datasource
{

typedef Data::RenewableCluster RenewableClusterFromLib;
typedef std::list<RenewableClusterFromLib*> RenewableClusterList;
typedef std::map<wxString, RenewableClusterList> RenewableClusterMap;

namespace // anonymous
{

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

} // anonymous namespace

RenewableClustersByAlphaOrder::RenewableClustersByAlphaOrder(HTMLListbox::Component& parent) :
    ClustersByAlphaOrder(parent)
{}

//! Destructor
RenewableClustersByAlphaOrder::~RenewableClustersByAlphaOrder()
{}

void RenewableClustersByAlphaOrder::refresh(const wxString& search)
{
    pParent.clear();

    if (pArea)
    {
        RenewableClusterMap l;
        GetRenewableClusterMap(pArea, l, search);
        if (!l.empty())
        {
            RenewableClusterMap::iterator end = l.end();
            for (RenewableClusterMap::iterator i = l.begin(); i != end; ++i)
            {
                wxString s;
                s << wxStringFromUTF8(pArea->name);
                if (s.size() > 43)
                {
                    s.resize(40);
                    s += wxT("...");
                }
                if (i->first.empty())
                    pParent.add(new Antares::Component::HTMLListbox::Item::Group(
                      s << wxT(" / <i>* no group *</i>")));
                else
                    pParent.add(new Antares::Component::HTMLListbox::Item::Group(s << wxT(" / ")
                                                                                   << i->first));

                // Added the area as a result
                RenewableClusterList::iterator jend = i->second.end();
                i->second.sort(SortAlphaOrder());
                for (RenewableClusterList::iterator j = i->second.begin(); j != jend; ++j)
                    pParent.add(new Antares::Component::HTMLListbox::Item::RenewableClusterItem(*j));
            }
        }
    }
    pParent.invalidate();
}

RenewableClustersByAlphaReverseOrder::RenewableClustersByAlphaReverseOrder(HTMLListbox::Component& parent) :
    ClustersByAlphaReverseOrder(parent)
{}

//! Destructor
RenewableClustersByAlphaReverseOrder::~RenewableClustersByAlphaReverseOrder()
{}

void RenewableClustersByAlphaReverseOrder::refresh(const wxString& search)
{
    pParent.clear();

    if (pArea)
    {
        RenewableClusterMap l;
        GetRenewableClusterMap(pArea, l, search);
        if (!l.empty())
        {
            RenewableClusterMap::iterator end = l.end();
            for (RenewableClusterMap::iterator i = l.begin(); i != end; ++i)
            {
                if (i->first.empty())
                    pParent.add(new Antares::Component::HTMLListbox::Item::Group(
                      wxString() << wxStringFromUTF8(pArea->name)
                                 << wxT(" / <i>* no group *</i>")));
                else
                    pParent.add(new Antares::Component::HTMLListbox::Item::Group(
                      wxString() << wxStringFromUTF8(pArea->name) << wxT(" / ") << i->first));
                // Added the area as a result
                RenewableClusterList::iterator jend = i->second.end();
                i->second.sort(SortAlphaReverseOrder());
                for (RenewableClusterList::iterator j = i->second.begin(); j != jend; ++j)
                    pParent.add(new Antares::Component::HTMLListbox::Item::RenewableClusterItem(*j));
            }
        }
    }
    pParent.invalidate();
}

} // namespace Datasource
} // namespace HTMLListbox
} // namespace Component
} // namespace Antares
