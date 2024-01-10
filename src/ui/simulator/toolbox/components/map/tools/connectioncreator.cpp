/*
** Copyright 2007-2024, RTE (https://www.rte-france.com)
** See AUTHORS.txt
** SPDX-License-Identifier: MPL-2.0
** This file is part of Antares-Simulator,** Adequacy and Performance assesment for interconnected energy networks.
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

#include "connectioncreator.h"
#include "../settings.h"

namespace Antares
{
namespace Map
{
namespace Tool
{
ConnectionCreator::ConnectionCreator(Manager& manager) :
 Tool(manager, "images/16x16/interco_plus.png")
{
    pWidth = 20;
    pHeight = 20;
}

ConnectionCreator::~ConnectionCreator()
{
}

void ConnectionCreator::draw(DrawingContext& dc,
                             const bool mouseDown,
                             const wxPoint& position,
                             const wxPoint& absolute) const
{
    // Parent::draw
    Tool::draw(dc, mouseDown, position, absolute);

    if (mouseDown)
    {
        dc.device().SetPen(wxPen(Settings::connectionHighlighted, 1, wxPENSTYLE_SHORT_DASH));

        // Position
        const Item* it = pManager.find(position.x, position.y);
        const bool gotcha = (it && !it->selected() && it->type() == Item::tyNode);
        const wxPoint target = (gotcha) ? it->absolutePosition(dc) : absolute;

        // Draw
        dc.device().DrawLine(pX + pWidth / 2, pY + pHeight / 2, target.x, target.y);
        if (gotcha)
        {
            dc.device().SetPen(wxPen(Settings::selectionNodeBorder, 3));
            dc.device().SetBrush(wxBrush(wxColor(0, 0, 0), wxBRUSHSTYLE_TRANSPARENT));

            const Node* node = dynamic_cast<const Node*>(it);
            dc.device().DrawRectangle(target.x - node->cachedSize().x / 2 - 2,
                                      target.y - node->cachedSize().y / 2 - 2,
                                      node->cachedSize().x + 4,
                                      node->cachedSize().y + 4);
        }
        else
        {
            dc.device().SetBrush(wxBrush(Settings::connectionHighlighted));
            dc.device().DrawRectangle(target.x - 3, target.y - 3, 6, 6);
        }
    }
}

bool ConnectionCreator::onMouseUp(const int mx, const int my)
{
    Item* target = pManager.find(mx, my);
    const bool gotcha = (target && !target->selected() && target->type() == Item::tyNode);
    if (gotcha)
        pManager.addConnectionFromEachSelectedItem(target);
    return false;
}

} // namespace Tool
} // namespace Map
} // namespace Antares
