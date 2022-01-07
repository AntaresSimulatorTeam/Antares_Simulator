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

#include "spotlight.h"
#include "listbox-panel.h"
#include <wx/dcbuffer.h>
#include "../../lock.h"

using namespace Yuni;

namespace Antares
{
namespace Component
{
enum
{
    fontSize = 8,
    horizontalSpaceBetweenGroupsAndItems = 6,
};
static const wxFont font(wxFontInfo(fontSize).Family(wxFONTFAMILY_SWISS).FaceName("Tahoma"));

// Speeds up the scrolling in list box
static const int NbPixelsPerVerticalScrollUnit = 10;

BEGIN_EVENT_TABLE(ListboxPanel, wxScrolledWindow)
EVT_PAINT(ListboxPanel::onDraw)
EVT_ERASE_BACKGROUND(ListboxPanel::onEraseBackground)
EVT_MOTION(ListboxPanel::onMouseMotion)
EVT_LEAVE_WINDOW(ListboxPanel::onMouseLeave)
EVT_LEFT_UP(ListboxPanel::onMouseLeftUp)
EVT_LEFT_DCLICK(ListboxPanel::onMouseDblClick)
END_EVENT_TABLE()

ListboxPanel::ListboxPanel(Spotlight* parent, uint flags) :
 wxScrolledWindow(parent), pParent(parent), pRecomputeLonguestGroupNameSize(true), pFlags(flags)
{
    assert(parent);
    SetBackgroundStyle(wxBG_STYLE_CUSTOM); // Required by both GTK and Windows

    // Only vertical
    EnableScrolling(false, true);
    SetScrollRate(16, pParent->itemHeight());

    if (0 != (flags & Spotlight::optBkgWhite))
        SetBackgroundColour(wxColour(255, 255, 255));

    const wxColour& bkgcol = GetBackgroundColour();
    pDisabledColor.Set(bkgcol.Red() - 35, bkgcol.Green() - 35, bkgcol.Blue() - 15);
    pMouseOverItem = (uint)-1;
}

ListboxPanel::~ListboxPanel()
{
    destroyBoundEvents();
}

void ListboxPanel::recomputeBoundsForLonguestGroupName(wxDC& dc)
{
    pRecomputeLonguestGroupNameSize = false;
    pLonguestGroupNameSize = dc.GetTextExtent(pLonguestGroupName);
    pLonguestGroupNameSize.SetWidth(pLonguestGroupNameSize.GetWidth() + 23);
}

void ListboxPanel::onMouseMotion(wxMouseEvent& evt)
{
    if (GUIIsLock())
        return;

    long y = evt.GetY();
    int vy;
    GetViewStart(nullptr, &vy); // Gives view start in scroll units, not in pixels
    // Convert view start height into pixels
    y += vy * NbPixelsPerVerticalScrollUnit;

    if (!(pFlags & Spotlight::optNoSearchInput))
    {
        if (y > searchResultTextHeight)
        {
            uint item = (uint)((y - searchResultTextHeight) / pParent->itemHeight());
            if (item != pMouseOverItem)
            {
                pMouseOverItem = item;
                Refresh();
            }
        }
        else
        {
            if (pMouseOverItem != (uint)-1)
            {
                pMouseOverItem = (uint)-1;
                Refresh();
            }
        }
    }
    else
    {
        uint item = (uint)((y) / pParent->itemHeight());
        if (item != pMouseOverItem)
        {
            pMouseOverItem = item;
            Refresh();
        }
    }

    // Notify other components
    Component::Panel::OnMouseMoveFromExternalComponent();
}

void ListboxPanel::onMouseLeave(wxMouseEvent&)
{
    pMouseOverItem = (uint)-1;
    Refresh();
}

void ListboxPanel::resetProvider(Spotlight::IProvider::Ptr provider)
{
    pProvider = provider;
}

void ListboxPanel::onDraw(wxPaintEvent&)
{
    Spotlight::IItem::VectorPtr itemsptr = pItems;
    Spotlight::SearchToken::VectorPtr tokensptr = pTokens;
    if (!pItems)
        return;
    // pProvider->redoResearch();
    // all items
    auto& items = *itemsptr;
    // item height
    uint itemHeight = pParent->itemHeight();

    // Bounding box
    wxRect bounds = GetRect();
    int maxHeight;
    GetClientSize(nullptr, &maxHeight);
    int offsetY;
    GetViewStart(nullptr, &offsetY); // Gives view start in scroll units, not in pixels
    // Convert view start height into pixels
    offsetY *= NbPixelsPerVerticalScrollUnit;
    maxHeight += offsetY;

    // The DC
    wxAutoBufferedPaintDC dc(this);
    // Shifts the device origin so we don't have to worry
    // about the current scroll position ourselves
    PrepareDC(dc);
    // Font
    dc.SetFont(font);

    // Bounds of the longuest group name
    if (pRecomputeLonguestGroupNameSize)
        recomputeBoundsForLonguestGroupName(dc);

    dc.SetPen(wxPen(GetBackgroundColour(), 1, wxPENSTYLE_SOLID));
    dc.SetBrush(wxBrush(GetBackgroundColour(), wxBRUSHSTYLE_SOLID));

    pLastGroupName.clear();
    bounds.x = 0;
    bounds.y = 0;
    int newHeight = (int)pItems->size() * (int)itemHeight;
    if (!(pFlags & Spotlight::optNoSearchInput))
        newHeight += searchResultTextHeight;
    if (newHeight > bounds.height)
        bounds.height = newHeight;
    // The scrollbars have height + 2 pixels
    bounds.height += 2;

    // Erasing the background
    dc.DrawRectangle(bounds);

    // Taking into consideration the Scrollbar size
    {
        wxSize clientSize = GetClientSize();
        bounds.width = clientSize.GetWidth() - 1 - 2 /*border*/;
    }

    // How many results
    if (!(pFlags & Spotlight::optNoSearchInput))
    {
        dc.SetTextForeground(pDisabledColor);
        pCacheResultTextSize = dc.GetTextExtent(pCacheResultText);
        dc.DrawText(pCacheResultText, 28, 6 - pCacheResultTextSize.GetHeight() / 2);

        bounds.y += searchResultTextHeight;
    }
    else
        bounds.y += 2; // for beauty

    uint count = (uint)pItems->size();

    for (uint i = 0; i != count; ++i)
    {
        // Do not draw outside the bounding box
        if ((uint)bounds.y + itemHeight + 10 /*arbitrary value*/ < (uint)offsetY)
        {
            bounds.y += itemHeight;
            if ((0 != (pFlags & Spotlight::optGroups)))
            {
                auto& item = *(items[i]);
                auto& group = item.group();
                if (!(!group) && group != pLastGroupName)
                    pLastGroupName = group;
            }
            continue;
        }

        auto& item = *(items[i]);
        auto& group = item.group();

        // Draw of the group name if the flag is enabled
        if ((0 != (pFlags & Spotlight::optGroups)))
        {
            if (!(!group) && group != pLastGroupName)
            {
                // Drawing the new group
                bounds.x = 0;
                pLastGroupName = group;
                wxString wxgroup = wxStringFromUTF8(group);
                wxSize groupBounds = dc.GetTextExtent(wxgroup);

                // Draw the group name
                dc.SetTextBackground(wxColour(0, 0, 0, wxALPHA_TRANSPARENT));
                dc.SetTextForeground(wxColour(23, 23, 75));

                int x = bounds.x + pLonguestGroupNameSize.GetWidth()
                        - horizontalSpaceBetweenGroupsAndItems - groupBounds.GetWidth();
                int y = bounds.y + 9 - groupBounds.GetHeight() / 2 - 1;
                if (System::unix)
                    ++y; // on unix, the name is not centered

                dc.DrawText(wxgroup, x, y);
            }
            bounds.x = pLonguestGroupNameSize.GetWidth();
        }
        else
            bounds.x = 3;

        item.draw(dc, itemHeight, bounds, (pMouseOverItem == i), tokensptr);

        // It is useless to draw outside the region and it slows down the program
        if (bounds.y > maxHeight)
            break;
    }
}

void ListboxPanel::updateItems(const Spotlight::IItem::VectorPtr& vptr,
                               const Spotlight::SearchToken::VectorPtr& tokens)
{
    uint maxSize = 0;
    uint itemIndex = (uint)-1;
    auto& items = *vptr;

    uint count = (!vptr) ? 0 : (uint)items.size();
    uint resultCount = 0;
    for (uint i = 0; i != count; ++i)
    {
        auto& item = *(items[i]);
        auto& group = item.group();

        if (item.countedAsResult())
            ++resultCount;
        if (group.size() > maxSize)
        {
            maxSize = group.size();
            itemIndex = i;
        }
    }

    if (itemIndex < count)
        pLonguestGroupName = wxStringFromUTF8((items[itemIndex]->group()));
    else
        pLonguestGroupName.clear();

    // Resetting the scrollbars
    int NbVerticalScrollUnitsInWindow = 0;
    if (!(pFlags & Spotlight::optNoSearchInput))
    {
        NbVerticalScrollUnitsInWindow = (count*pParent->itemHeight() + searchResultTextHeight + 2) / NbPixelsPerVerticalScrollUnit;
        SetScrollbars(1, NbPixelsPerVerticalScrollUnit, 1, NbVerticalScrollUnitsInWindow);
    }
    else
    {
        NbVerticalScrollUnitsInWindow = (count*pParent->itemHeight() + 2) / NbPixelsPerVerticalScrollUnit;
        SetScrollbars(1, NbPixelsPerVerticalScrollUnit, 1, NbVerticalScrollUnitsInWindow);
    }

    pItems = vptr;
    pTokens = tokens;
    pRecomputeLonguestGroupNameSize = true;

    pCacheResultText.clear();
    switch (resultCount)
    {
    case 0:
        pCacheResultText << wxT("No item found");
        break;
    case 1:
        pCacheResultText << wxT("1 item found");
        break;
    default:
        pCacheResultText << resultCount << wxT(" items found");
    }

    // Dispatcher::GUI::Refresh(this);
    Refresh();
}

void ListboxPanel::onMouseLeftUp(wxMouseEvent& evt)
{
    if (not GUIIsLock())
        Dispatcher::GUI::Post(this, &ListboxPanel::doSelectItemFromMouseLeftUp);
    evt.Skip();
}

void ListboxPanel::onMouseDblClick(wxMouseEvent& evt)
{
    if (not GUIIsLock())
        Dispatcher::GUI::Post(this, &ListboxPanel::doSelectItemFromMouseDblClick);
    evt.Skip();
}

void ListboxPanel::doSelectItemFromMouseDblClick()
{
    if (GUIIsLock())
        return;

    Spotlight::IItem::VectorPtr itemsptr = pItems;
    Spotlight::IItem::Vector& items = *itemsptr;
    const uint itemIndex = pMouseOverItem;

    if (itemIndex < items.size())
    {
        Spotlight::IItem::Ptr itemptr = items[itemIndex];
        if (!(!itemptr))
        {
            Spotlight::IProvider::Ptr provider = pProvider;
            if (!provider)
            {
                // If no provider is available, impossible to do
                // anything
                //
                // The selection, if any, will remain as it
            }
            else
            {
                // If the provider accepts the new selection, we have to update
                // internal flags
                if (provider->onDoubleClickSelect(itemptr))
                {
                    for (uint i = 0; i != (uint)items.size(); ++i)
                    {
                        if (!(!items[i]))
                            items[i]->unselect();
                    }
                    itemptr->select();
                }
            }
            Dispatcher::GUI::Refresh(this);
        }
    }
}

void ListboxPanel::doSelectItemFromMouseLeftUp()
{
    if (GUIIsLock())
        return;

    Spotlight::IItem::VectorPtr itemsptr = pItems;
    Spotlight::IItem::Vector& items = *itemsptr;
    uint itemIndex = pMouseOverItem;

    if (itemIndex < items.size())
    {
        Spotlight::IItem::Ptr itemptr = items[itemIndex];
        if (!(!itemptr))
        {
            Spotlight::IProvider::Ptr provider = pProvider;
            if (!provider)
            {
                // If no provider is available, impossible to do
                // anything
                //
                // The selection, if any, will remain as it
            }
            else
            {
                // If the provider accepts the new selection, we have to update
                // internal flags
                if (provider->onSelect(itemptr))
                {
                    for (uint i = 0; i != (uint)items.size(); ++i)
                    {
                        if (!(!items[i]))
                            items[i]->unselect();
                    }
                    itemptr->select();
                }
            }
            Dispatcher::GUI::Refresh(this);
        }
    }
}

} // namespace Component
} // namespace Antares
