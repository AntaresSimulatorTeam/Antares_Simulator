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

#include "wip-panel.h"
#include "../resources.h"
#include <wx/dcbuffer.h>
#include <wx/settings.h>
#include <wx/sizer.h>
#include "../../application/main.h"

using namespace Yuni;

namespace Antares
{
namespace Component
{
BEGIN_EVENT_TABLE(WIPPanel, Panel)
EVT_PAINT(WIPPanel::onDraw)
EVT_ERASE_BACKGROUND(WIPPanel::onEraseBackground)
END_EVENT_TABLE()

WIPPanel::WIPPanel(wxWindow* parent) : Panel(parent)
{
    SetBackgroundStyle(wxBG_STYLE_CUSTOM); // Required by both GTK and Windows
    SetSize(30, 18);
    pIcon = Resources::BitmapLoadFromFile("images/16x16/hourglass.png");
}

WIPPanel::~WIPPanel()
{
    delete pIcon;
}

void WIPPanel::onDraw(wxPaintEvent&)
{
    // The DC
    wxAutoBufferedPaintDC dc(this);
    // Shifts the device origin so we don't have to worry
    // about the current scroll position ourselves
    PrepareDC(dc);

    // Drawing the background
    wxRect rect = GetRect();
    const wxColour& originalColor = GetBackgroundColour();

    // Redraw the background
    dc.SetPen(wxPen(originalColor, 1, wxPENSTYLE_SOLID));
    dc.SetBrush(wxBrush(originalColor, wxBRUSHSTYLE_SOLID));
    dc.DrawRectangle(0, 0, rect.width, rect.height);

    Forms::ApplWnd* mainFrm = Forms::ApplWnd::Instance();
    if (mainFrm && mainFrm->isInWIPMode() && pIcon)
    {
        // dc.SetPen(wxPen(wxColour(140, 140, 140), 1, wxSOLID));
        // dc.DrawLine(0, 4, 0, rect.height - 4);
        dc.DrawBitmap(*pIcon, rect.width / 2 - 16 / 2, rect.height / 2 - 16 / 2, true);
    }
}

} // namespace Component
} // namespace Antares
