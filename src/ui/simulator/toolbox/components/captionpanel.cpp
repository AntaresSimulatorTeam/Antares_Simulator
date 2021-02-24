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

#include "captionpanel.h"
#include <wx/dcbuffer.h>
#include <yuni/core/math.h>

#define NODE_DRAW_COLOR_VARIATION_LIGHT 10
#define NODE_DRAW_COLOR_VARIATION_DARK 5

using namespace Yuni;

namespace Antares
{
namespace Component
{
BEGIN_EVENT_TABLE(CaptionPanel, Panel)
EVT_PAINT(CaptionPanel::onDraw)
EVT_SIZE(CaptionPanel::onResize)
END_EVENT_TABLE()

namespace // anonymous
{
// The font re-used for each drawing
enum
{
    fontSize = 8,
};
const wxFont font(wxFontInfo(fontSize).Family(wxFONTFAMILY_SWISS).FaceName("Tahoma"));
} // anonymous namespace

CaptionPanel::CaptionPanel(wxWindow* parent) : Panel(parent), pCaption()
{
    SetSize(100, 20);
    SetBackgroundStyle(wxBG_STYLE_CUSTOM); // Required by Windows
}

CaptionPanel::CaptionPanel(wxWindow* parent, const wxString& caption) :
 Panel(parent), pCaption(caption)
{
    SetMinSize(wxSize(100, 20));
    SetBackgroundStyle(wxBG_STYLE_CUSTOM); // Required by Windows
}

CaptionPanel::~CaptionPanel()
{
}

void CaptionPanel::caption(const wxString& s)
{
    pCaption = s;
    Refresh();
}

void CaptionPanel::onDraw(wxPaintEvent&)
{
    // The DC
    wxAutoBufferedPaintDC dc(this);
    // Shifts the device origin so we don't have to worry
    // about the current scroll position ourselves
    PrepareDC(dc);

    // Cute font
    dc.SetFont(font);

    // Drawing the background
    /*
    const wxRect pRect = this->GetClientSize();
    dc.SetPen(wxPen(wxColour(71,78,86), 1));
    dc.SetBrush(wxBrush(wxColour(108,119,131)));
    dc.DrawRectangle(0, 0, pRect.width - 1, pRect.height - 1);
    */

    wxColour ori(101, 109, 123);
    const wxRect pRect = this->GetClientSize();

    wxColour a1, b1, a2, b2;
    double y = ori.Red() * 0.299 + ori.Green() * 0.587 + ori.Blue() * 0.114;
    double u = (ori.Blue() - y) * 0.565;
    double v = (ori.Red() - y) * 0.713;
    if (y < 50.)
        y = 50.;
    else
    {
        if (y > 225.)
            y = 225.;
    }

    const int r = Math::MinMax<int>((int)(y + 1.403 * v), 0, 255);
    const int g = Math::MinMax<int>((int)(y - 0.344 * u - 0.714 * v), 0, 255);
    const int b = Math::MinMax<int>((int)(y + 1.770 * u), 0, 255);

    a1.Set((unsigned char)Math::MinMax<int>(r + NODE_DRAW_COLOR_VARIATION_LIGHT / 2, 0, 255),
           (unsigned char)Math::MinMax<int>(g + NODE_DRAW_COLOR_VARIATION_LIGHT / 2, 0, 255),
           (unsigned char)Math::MinMax<int>(b + NODE_DRAW_COLOR_VARIATION_LIGHT / 2, 0, 255));
    b1.Set((unsigned char)Math::MinMax<int>(r - NODE_DRAW_COLOR_VARIATION_DARK / 2, 0, 255),
           (unsigned char)Math::MinMax<int>(g - NODE_DRAW_COLOR_VARIATION_DARK / 2, 0, 255),
           (unsigned char)Math::MinMax<int>(b - NODE_DRAW_COLOR_VARIATION_DARK / 2, 0, 255));
    b2.Set((unsigned char)Math::MinMax<int>(r + NODE_DRAW_COLOR_VARIATION_LIGHT * 2, 0, 255),
           (unsigned char)Math::MinMax<int>(g + NODE_DRAW_COLOR_VARIATION_LIGHT * 2, 0, 255),
           (unsigned char)Math::MinMax<int>(b + NODE_DRAW_COLOR_VARIATION_LIGHT * 2, 0, 255));
    a2.Set((unsigned char)Math::MinMax<int>(r - NODE_DRAW_COLOR_VARIATION_DARK, 0, 255),
           (unsigned char)Math::MinMax<int>(g - NODE_DRAW_COLOR_VARIATION_DARK, 0, 255),
           (unsigned char)Math::MinMax<int>(b - NODE_DRAW_COLOR_VARIATION_DARK, 0, 255));

    // Gradient inside
    dc.GradientFillLinear(wxRect(pRect.x, pRect.y, pRect.width, pRect.height / 2), a1, b1, wxSOUTH);
    dc.GradientFillLinear(
      wxRect(pRect.x, pRect.y + pRect.height / 2, pRect.width, pRect.height / 2), a2, b2, wxSOUTH);
    dc.SetPen(wxPen(a2, 1, wxPENSTYLE_SOLID));
    dc.DrawLine(0, pRect.height - 1, pRect.width, pRect.height - 1);

    // Drawing the Caption in the middle of the panel
    {
        const wxSize textExtent = dc.GetTextExtent(pCaption);
        wxPoint p(pRect.width / 2 - textExtent.GetWidth() / 2,
                  pRect.height / 2 - textExtent.GetHeight() / 2);
        p.y += 1;
        dc.SetTextForeground(wxColour(0, 0, 0));
        dc.DrawText(pCaption, p.x, p.y);
        dc.SetTextForeground(wxColour(255, 255, 255));
        dc.DrawText(pCaption, p.x - 1, p.y - 1);
    }
}

void CaptionPanel::onResize(wxSizeEvent& evt)
{
    Dispatcher::GUI::Refresh(this);
    evt.Skip();
}

} // namespace Component
} // namespace Antares
