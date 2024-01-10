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
** There are special exceptions to the terms and conditions of the
** license as they are applied to this software. View the full text of
** the exceptions in file COPYING.txt in the directory of this software
** distribution
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/

#pragma once

#include "datasource.h"
#include <yuni/core/event.h>
#include <antares/study/study.h>
#include "../item/cluster-item.h"

using namespace Antares::Component::HTMLListbox::Item;

namespace Antares
{
namespace Component
{
namespace HTMLListbox
{
namespace Datasource
{
struct SortAlphaOrder
{
    inline bool operator()(const Data::Cluster* a, const Data::Cluster* b)
    {
        return a->name() < b->name();
    }
};

struct SortAlphaReverseOrder
{
    inline bool operator()(const Data::Cluster* a, const Data::Cluster* b)
    {
        return a->name() > b->name();
    }
};

wxString groupNameToGroupTitle(Data::Area* area, wxString& groupName);

class ClustersByOrder : public Yuni::IEventObserver<ClustersByOrder>, public IDatasource
{
public:
    //! \name Constructor & Destructor
    //@{
    //! Default Constructor
    ClustersByOrder(HTMLListbox::Component& parent);
    //! Destructor
    virtual ~ClustersByOrder();
    //@}

    virtual wxString name() const = 0;

    virtual const char* icon() const = 0;

    virtual void refresh(const wxString& search = wxEmptyString);

    void onAreaChanged(Data::Area* area);
    void onInvalidateAllAreas();

    void hasGroupChanged(bool b);
    bool hasGroupChanged() const;

private:
    virtual void reorderItemsList(const wxString& search) = 0;
    virtual void rebuildItemsList(const wxString& search) = 0;

protected:
    Data::Area* pArea;
    bool hasGroupJustChanged_;
    std::map<wxString, IItem::Ptr> groups_to_items_;
    std::map<Data::Cluster*, ClusterItem::Ptr> pClustersToItems;

}; // class ClustersByOrder

} // namespace Datasource
} // namespace HTMLListbox
} // namespace Component
} // namespace Antares
