// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0


#include "tool.h"
#include "../../../resources.h"
#include "../settings.h"

namespace Antares::Map::Tool
{
Tool::Tool(Manager& manager, const char* icon):
    pManager(manager),
    pX(0),
    pY(0),
    pWidth(20),
    pHeight(20)
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
    {
        dc.device().DrawBitmap(*pIcon, pX + 2, pY + 1, true);
    }
}

} // namespace Antares::Map::Tool
