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

#include "thermal-cluster.h"
#include "../../../../application/study.h"
#include "../item/thermal-cluster-item.h"
#include "../item/group.h"
#include "../component.h"
#include <map>
#include <list>
#include "../../../../application/main.h"

using namespace Yuni;

namespace Antares
{
namespace Component
{
namespace HTMLListbox
{
namespace Datasource
{
namespace ThermalClusters
{
typedef Data::ThermalCluster ThermalClusterFromLib;
typedef std::list<ThermalClusterFromLib*> ThermalClusterList;
typedef std::map<wxString, ThermalClusterList> ThermalClusterMap;

namespace // anonymous
{
struct SortAlphaOrder
{
    inline bool operator()(const Data::ThermalCluster* a, const Data::ThermalCluster* b)
    {
        return a->name() < b->name();
    }
};

struct SortAlphaReverseOrder
{
    inline bool operator()(const Data::ThermalCluster* a, const Data::ThermalCluster* b)
    {
        return a->name() > b->name();
    }
};

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

} // anonymous namespace

ByAlphaOrder::ByAlphaOrder(HTMLListbox::Component& parent) : IDatasource(parent), pArea(nullptr)
{
    OnStudyAreasChanged.connect(this, &ByAlphaOrder::onInvalidateAllAreas);
    Forms::ApplWnd::Instance()->onApplicationQuit.connect(this,
                                                          &ByAlphaOrder::onInvalidateAllAreas);
}

//! Destructor
ByAlphaOrder::~ByAlphaOrder()
{
    destroyBoundEvents();
}

void ByAlphaOrder::refresh(const wxString& search)
{
    pParent.clear();

    if (pArea)
    {
        ThermalClusterMap l;
        GetThermalClusterMap(pArea, l, search);
        if (!l.empty())
        {
            ThermalClusterMap::iterator end = l.end();
            for (ThermalClusterMap::iterator i = l.begin(); i != end; ++i)
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
                ThermalClusterList::iterator jend = i->second.end();
                i->second.sort(SortAlphaOrder());
                for (ThermalClusterList::iterator j = i->second.begin(); j != jend; ++j)
                    pParent.add(new Antares::Component::HTMLListbox::Item::ThermalClusterItem(*j));
            }
        }
    }
    pParent.invalidate();
}

void ByAlphaOrder::onAreaChanged(Data::Area* area)
{
    pArea = area;
}

void ByAlphaOrder::onInvalidateAllAreas()
{
    pArea = nullptr;
}

ByAlphaReverseOrder::ByAlphaReverseOrder(HTMLListbox::Component& parent) :
 IDatasource(parent), pArea(nullptr)
{
    OnStudyAreasChanged.connect(this, &ByAlphaReverseOrder::onInvalidateAllAreas);
    Forms::ApplWnd::Instance()->onApplicationQuit.connect(
      this, &ByAlphaReverseOrder::onInvalidateAllAreas);
}

//! Destructor
ByAlphaReverseOrder::~ByAlphaReverseOrder()
{
    destroyBoundEvents();
}

void ByAlphaReverseOrder::refresh(const wxString& search)
{
    pParent.clear();

    if (pArea)
    {
        ThermalClusterMap l;
        GetThermalClusterMap(pArea, l, search);
        if (!l.empty())
        {
            ThermalClusterMap::iterator end = l.end();
            for (ThermalClusterMap::iterator i = l.begin(); i != end; ++i)
            {
                if (i->first.empty())
                    pParent.add(new Antares::Component::HTMLListbox::Item::Group(
                      wxString() << wxStringFromUTF8(pArea->name)
                                 << wxT(" / <i>* no group *</i>")));
                else
                    pParent.add(new Antares::Component::HTMLListbox::Item::Group(
                      wxString() << wxStringFromUTF8(pArea->name) << wxT(" / ") << i->first));
                // Added the area as a result
                ThermalClusterList::iterator jend = i->second.end();
                i->second.sort(SortAlphaReverseOrder());
                for (ThermalClusterList::iterator j = i->second.begin(); j != jend; ++j)
                    pParent.add(new Antares::Component::HTMLListbox::Item::ThermalClusterItem(*j));
            }
        }
    }
    pParent.invalidate();
}

void ByAlphaReverseOrder::onAreaChanged(Data::Area* area)
{
    pArea = area;
}

void ByAlphaReverseOrder::onInvalidateAllAreas()
{
    pArea = nullptr;
}

} // namespace ThermalClusters
} // namespace Datasource
} // namespace HTMLListbox
} // namespace Component
} // namespace Antares
