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

#include "tool.h"
#include "../../../resources.h"
#include "../settings.h"

namespace Antares
{
namespace Map
{
namespace Tool
{
Tool::Tool(Manager& manager, const char* icon) :
 pManager(manager), pX(0), pY(0), pWidth(20), pHeight(20)
{
    pIcon = (icon && *icon != '\0') ? Resources::BitmapLoadFromFile(icon) : nullptr;
}

Tool::~Tool()
{
    delete pIcon;
}

void Tool::icon(const char* filename)
{
    delete pIcon;
    pIcon = (filename && *filename != '\0') ? Resources::BitmapLoadFromFile(filename) : nullptr;
}

void Tool::draw(DrawingContext& dc, const bool mouseDown, const wxPoint&, const wxPoint&) const
{
    if (!mouseDown)
    {
        dc.device().SetPen(wxPen(Settings::selectionBoxBorder));
        dc.device().SetBrush(wxBrush(Settings::selectionBoxBackground));
    }
    else
    {
        dc.device().SetPen(wxPen(Settings::selectionBoxBorderHighlighted));
        dc.device().SetBrush(wxBrush(Settings::selectionBoxBackgroundHighlighted));
    }

    dc.device().DrawRoundedRectangle(pX, pY, pWidth, pHeight, 2);
    if (pIcon)
        dc.device().DrawBitmap(*pIcon, pX + 2, pY + 1, true);
}

} // namespace Tool
} // namespace Map
} // namespace Antares
