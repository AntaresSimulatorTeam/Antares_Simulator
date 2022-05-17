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

#include "bindingconstraint.h"
#include "../component.h"
#include "../../../../application/study.h"

namespace Antares
{
namespace Map
{
BindingConstraint::BindingConstraint(Manager& manager) : Item(manager, 100000)
{
}

BindingConstraint::~BindingConstraint()
{
    clear();
}

void BindingConstraint::TextPart::refreshCache(wxDC& dc)
{
    this->size = dc.GetTextExtent(text);
}

void BindingConstraint::draw(DrawingContext&)
{
}

void BindingConstraint::drawExternalDrawer(DrawingContext& dc)
{
    static const int space = 5;
    wxDC& device = dc.device();

    wxPoint pCachedPosition(0, 0);
    wxPoint pCachedSize(0, 0);

    pTextParts.clear();
    pTextParts.push_back(TextPart(wxT("Constraint name"), 255, 220, 220));
    pTextParts.push_back(TextPart(wxT(" ≡ "), 255, 255, 255));

    pTextParts.push_back(TextPart(wxT("2.5"), 255, 255, 255));
    pTextParts.push_back(TextPart(wxT("×"), 255, 138, 40));
    pTextParts.push_back(TextPart(wxT("("), 255, 138, 40));
    pTextParts.push_back(TextPart(wxT("Allemagne"), 200, 200, 255));
    pTextParts.push_back(TextPart(wxT("→"), 164, 255, 255));
    pTextParts.push_back(TextPart(wxT("Suisse"), 200, 200, 255));
    pTextParts.push_back(TextPart(wxT(")"), 255, 138, 40));

    pTextParts.push_back(TextPart(wxT("  +  "), 255, 138, 40));

    pTextParts.push_back(TextPart(wxT("7.2"), 255, 255, 255));
    pTextParts.push_back(TextPart(wxT("×"), 255, 138, 40));
    pTextParts.push_back(TextPart(wxT("("), 255, 138, 40));
    pTextParts.push_back(TextPart(wxT("Allemagne"), 200, 200, 255));
    pTextParts.push_back(TextPart(wxT("→"), 164, 255, 255));
    pTextParts.push_back(TextPart(wxT("Suisse"), 200, 200, 255));
    pTextParts.push_back(TextPart(wxT(")"), 255, 138, 40));

    pTextParts.push_back(TextPart(wxT("  ≤ "), 255, 138, 40));
    pTextParts.push_back(TextPart(wxT("10000.7 MW/h"), 183, 255, 170));

    std::vector<TextPart>::iterator end = pTextParts.end();

    // size
    for (std::vector<TextPart>::iterator i = pTextParts.begin(); i != end; ++i)
    {
        i->refreshCache(device);
        if (pCachedSize.y < i->size.GetHeight())
            pCachedSize.y = i->size.GetHeight();
        pCachedSize.x += i->size.GetWidth() + 2;
    }

    pCachedSize.y += 2 * space;
    pCachedSize.x += 4 * space;
    // pCachedPosition.y += pCachedSize.y / 2;

    pCachedPosition.x += dc.scroll().x;
    pCachedPosition.y = 15;

    pCachedPosition.x += dc.clientSize().x / 2 - pCachedSize.x / 2;

    wxRect pCachedBB(
      pCachedPosition.x, pCachedPosition.y - pCachedSize.y / 2, pCachedSize.x, pCachedSize.y);

    // Color
    device.SetPen(wxPen(wxColour(0, 0, 0)));
    device.SetBrush(wxBrush(wxColour(70, 70, 70)));

    pCachedBB.SetHeight(pCachedBB.GetHeight() + 24);
    device.DrawRectangle(0, 0, dc.clientSize().x, dc.clientSize().y);

    int pos = pCachedPosition.x + 2 * space;
    for (std::vector<TextPart>::iterator i = pTextParts.begin(); i != end; ++i)
    {
        device.SetTextForeground(wxColour(20, 20, 20));
        // device.DrawText(i->text, pos, 1+pCachedPosition.y - i->size.GetHeight() / 2);
        // device.DrawText(i->text, 1+pos, pCachedPosition.y - i->size.GetHeight() / 2);
        device.DrawText(i->text, 1 + pos, 1 + pCachedPosition.y - i->size.GetHeight() / 2);
        // device.DrawText(i->text, -1+pos, -1+pCachedPosition.y - i->size.GetHeight() / 2);
        device.SetTextForeground(i->color);
        device.DrawText(i->text, pos, pCachedPosition.y - i->size.GetHeight() / 2);
        pos += i->size.GetWidth() + 2;
    }
}

void BindingConstraint::refreshCache(wxDC&)
{
    pInvalidated = false;
}

void BindingConstraint::selected(bool v)
{
    if (v != pSelected)
    {
        if (v)
        {
            // We keep the information about the selection of the connections
            // To restore them when unselected
            Connections::iterator end = pConnections.end();
            for (Connections::iterator i = pConnections.begin(); i != end; ++i)
            {
                i->second.selected = i->first->selected();
                i->first->selected(true);
                i->first->caption(wxString() << i->second.weight);
            }

            // Add itself to the list of selected items
            pManager.pSelectedItems[this] = true;
            pManager.pExternalQueue.push(this);
            pManager.pComponent.drawerVisible();
        }
        else
        {
            pManager.pComponent.drawerVisible();
            pManager.pExternalQueue.pop();
            Connections::iterator end = pConnections.end();
            for (Connections::iterator i = pConnections.begin(); i != end; ++i)
            {
                i->first->selected(i->second.selected);
                i->first->caption(wxString());
            }

            // Remove itself from the list of selected items
            Manager::NodeMap::iterator i = pManager.pSelectedItems.find(this);
            if (i != pManager.pSelectedItems.end())
                pManager.pSelectedItems.erase(i);
        }
        pSelected = v;
        invalidate();
    }
}

void BindingConstraint::add(Item* item)
{
    if (item && dynamic_cast<Connection*>(item))
    {
        Connection* c = dynamic_cast<Connection*>(item);
        if (pConnections.find(c) != pConnections.end())
        {
            pConnections.insert(std::pair<Connection*, Infos>(c, Infos()));
            if (item->pLinks)
                (*(item->pLinks))[this] = false;
            MarkTheStudyAsModified();
            GUIFlagInvalidateAreas = true;
        }
    }
}

bool BindingConstraint::remove(Item* item)
{
    if (item && dynamic_cast<Connection*>(item))
    {
        Connection* c = dynamic_cast<Connection*>(item);
        Connections::iterator i = pConnections.find(c);
        if (i != pConnections.end())
        {
            if (item->pLinks)
            {
                Links::iterator i = item->pLinks->find(this);
                if (i != item->pLinks->end())
                    item->pLinks->erase(i);
            }
            pConnections.erase(i);
            MarkTheStudyAsModified();
            GUIFlagInvalidateAreas = true;
            return true;
        }
    }
    return false;
}

void BindingConstraint::clear()
{
    if (!pConnections.empty())
    {
        Connections::iterator end = pConnections.end();
        for (Connections::iterator i = pConnections.begin(); i != end; ++i)
        {
            if (i->first->pLinks)
            {
                Links::iterator j = ((i->first)->pLinks)->find(this);
                if (j != (i->first)->pLinks->end())
                    (i->first)->pLinks->erase(j);
            }
        }
        pConnections.clear();
        GUIFlagInvalidateAreas = true;
    }
}

} // namespace Map
} // namespace Antares
