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
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/

#include "bindingconstraint.h"
#include "../../../../application/study.h"
#include "../item/bindingconstraint.h"
#include "../item/group.h"
#include "../component.h"
#include <map>
#include <list>
#include "../../../../application/main.h"

namespace Antares
{
namespace Component
{
namespace HTMLListbox
{
namespace Datasource
{
namespace BindingConstraints
{
using BindingConstraintFromLib = Data::BindingConstraint;
using BindingConstraintList = std::list<BindingConstraintFromLib*>;

namespace // anonymous
{
struct SortAlphaOrder
{
    inline bool operator()(const Data::BindingConstraint* a, const Data::BindingConstraint* b)
    {
        return a->name() < b->name();
    }
};

struct SortAlphaReverseOrder
{
    inline bool operator()(const Data::BindingConstraint* a, const Data::BindingConstraint* b)
    {
        return a->name() > b->name();
    }
};

void GetBindingConstraintList(Data::Study& study, BindingConstraintList& l, const wxString& search)
{
    Data::BindingConstraintsRepository::iterator end = study.bindingConstraints.end();
    for (Data::BindingConstraintsRepository::iterator i = study.bindingConstraints.begin(); i != end; ++i)
    {
        if (search.empty())
            l.push_back(*i);
    }
}

} // anonymous namespace

ByAlphaOrder::ByAlphaOrder(HTMLListbox::Component& parent) : IDatasource(parent)
{
}

//! Destructor
ByAlphaOrder::~ByAlphaOrder()
{
    destroyBoundEvents();
}

void ByAlphaOrder::refresh(const wxString& search)
{
    pParent.clear();

    if (CurrentIsValid())
    {
        BindingConstraintList l;
        GetBindingConstraintList(*GetCurrentStudy(), l, search);
        if (!l.empty())
        {
            l.sort(SortAlphaOrder());
            // Added the area as a result
            auto jend = l.end();
            for (auto j = l.begin(); j != jend; ++j)
                pParent.add(
                  std::make_shared<Antares::Component::HTMLListbox::Item::BindingConstraint>(*j));
        }
    }
    pParent.forceReload();
}

ByAlphaReverseOrder::ByAlphaReverseOrder(HTMLListbox::Component& parent) : IDatasource(parent)
{
}

//! Destructor
ByAlphaReverseOrder::~ByAlphaReverseOrder()
{
    destroyBoundEvents();
}

void ByAlphaReverseOrder::refresh(const wxString& search)
{
    pParent.clear();

    if (CurrentIsValid())
    {
        BindingConstraintList l;
        GetBindingConstraintList(*GetCurrentStudy(), l, search);
        if (!l.empty())
        {
            l.sort(SortAlphaReverseOrder());
            // Added the area as a result
            BindingConstraintList::iterator jend = l.end();
            for (BindingConstraintList::iterator j = l.begin(); j != jend; ++j)
                pParent.add(
                  std::make_shared<Antares::Component::HTMLListbox::Item::BindingConstraint>(*j));
        }
    }
    pParent.forceReload();
}

} // namespace BindingConstraints
} // namespace Datasource
} // namespace HTMLListbox
} // namespace Component
} // namespace Antares
