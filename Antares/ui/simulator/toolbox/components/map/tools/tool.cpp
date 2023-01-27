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
    pIcon = (icon && *icon != '\0') ? Resources::BitmapLoadFromFile(icon) : NULL;
}

Tool::~Tool()
{
    delete pIcon;
}

void Tool::icon(const char* filename)
{
    delete pIcon;
    pIcon = (filename && *filename != '\0') ? Resources::BitmapLoadFromFile(filename) : NULL;
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
