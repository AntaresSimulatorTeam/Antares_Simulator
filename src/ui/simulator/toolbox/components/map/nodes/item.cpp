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

#include "item.h"
#include "../settings.h"
#include "bindingconstraint.h"

namespace Antares
{
namespace Map
{
Item::Item(Manager& manager, const int zPos) :
 pManager(manager),
 pColor(Settings::defaultNodeBackground),
 pSelected(false),
 pInvalidated(true),
 pX(0),
 pY(0),
 pCaption(),
 pZPosition(zPos)
{
    pManager.internalAddItem(this);
    pLinks = new Links();
}

Item::~Item()
{
    selected(false);
    pManager.setChangesFlag(true);
    pManager.internalRemoveItem(this);
    internalClearAllLinks();
    delete pLinks;
}

void Item::color(const wxColour& c)
{
    pColor = c;
    this->forceReload();
    colorHasChanged();
}

void Item::color(const wxString& s)
{
    pColor.Set(s);
    this->forceReload();
    colorHasChanged();
}

void Item::color(const int r, const int g, const int b)
{
    pColor.Set((unsigned char)Yuni::Math::MinMax<int>(r, 0, 255),
               (unsigned char)Yuni::Math::MinMax<int>(g, 0, 255),
               (unsigned char)Yuni::Math::MinMax<int>(b, 0, 255),
               wxALPHA_OPAQUE);
    this->forceReload();
    colorHasChanged();
}

void Item::color(const int r, const int g, const int b, const int alpha)
{
    pColor.Set((unsigned char)Yuni::Math::MinMax<int>(r, 0, 255),
               (unsigned char)Yuni::Math::MinMax<int>(g, 0, 255),
               (unsigned char)Yuni::Math::MinMax<int>(b, 0, 255),
               (unsigned char)Yuni::Math::MinMax<int>(alpha, 0, 255));
    this->forceReload();
    colorHasChanged();
}

void Item::selected(bool v)
{
    if (v != pSelected)
    {
        pSelected = v;
        if (v)
            pManager.pSelectedItems[this] = true;
        else
        {
            Manager::NodeMap::iterator i = pManager.pSelectedItems.find(this);
            if (i != pManager.pSelectedItems.end())
                pManager.pSelectedItems.erase(i);
        }
        forceReload();
    }
}

void Item::move(const int, const int)
{
    // Do nothing
    positionHasChanged();
}

void Item::internalClearAllLinks()
{
    if (pLinks && !pLinks->empty())
    {
        Links* lnks = pLinks;
        pLinks = NULL;

        const Links::const_iterator end = lnks->end();
        for (Links::const_iterator i = lnks->begin(); i != end; ++i)
        {
            if (dynamic_cast<BindingConstraint*>(i->first))
            {
                BindingConstraint* bc = dynamic_cast<BindingConstraint*>(i->first);
                bc->remove(this);
                if (bc->empty())
                    pManager.internalRemoveItem(bc);
            }
            else
                pManager.internalRemoveItem(i->first);
        }

        lnks->clear();
        pLinks = lnks;
    }
}

void Item::forceReload()
{
    pInvalidated = true;
    if (!pLinks->empty())
    {
        Links::iterator end = pLinks->end();
        for (Links::iterator i = pLinks->begin(); i != end; ++i)
            i->first->forceReload();
    }
}

wxPoint Item::absolutePosition(DrawingContext& dc) const
{
    return wxPoint(dc.origin().x + pX, dc.origin().y - pY);
}

} // namespace Map
} // namespace Antares
