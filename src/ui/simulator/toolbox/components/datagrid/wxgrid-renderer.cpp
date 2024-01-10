/*
** Copyright 2007-2024, RTE (https://www.rte-france.com)
** See AUTHORS.txt
** SPDX-License-Identifier: MPL-2.0
** This file is part of Antares-Simulator,
** Adequacy and Performance assesment for interconnected energy networks.
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

#include "wxgrid-renderer.h"
#include <wx/settings.h>
#include <wx/dc.h>

using namespace Yuni;

namespace Antares
{
namespace Component
{
namespace Datagrid
{
namespace // anonymous
{
// The font re-used for each drawing
enum
{
    fontSize = 8,
};
const wxFont font(wxFontInfo(fontSize).Family(wxFONTFAMILY_SWISS).FaceName("Tahoma"));

} // anonymous namespace

AntaresWxGridRenderer::AntaresWxGridRenderer() : wxGridCellFloatRenderer(-1, 3), renderer(nullptr)
{
}

void AntaresWxGridRenderer::Draw(wxGrid& grid,
                                 wxGridCellAttr& attr,
                                 wxDC& dc,
                                 const wxRect& r,
                                 int row,
                                 int col,
                                 bool isSelected)
{
    if (!renderer)
        return;
    wxRect rect(r);

    dc.SetFont(font);
    if (!isSelected)
    {
        dc.SetTextForeground(attr.GetTextColour());
        dc.SetPen(wxPen(attr.GetBackgroundColour(), 1, wxPENSTYLE_SOLID));
        dc.SetBrush(wxBrush(attr.GetBackgroundColour(), wxBRUSHSTYLE_SOLID));
        dc.DrawRectangle(rect.x, rect.y, rect.width, rect.height);
    }
    else
    {
        const wxColour& txtColor = wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHTTEXT);
        dc.SetTextForeground(txtColor);
        const wxColour& bkg = wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHT);
        dc.SetPen(wxPen(bkg, 1, wxPENSTYLE_SOLID));
        dc.SetBrush(wxBrush(bkg, wxBRUSHSTYLE_SOLID));
        dc.DrawRectangle(rect.x, rect.y, rect.width, rect.height);
    }

    // bottom border
    dc.SetPen(wxPen(renderer->horizontalBorderColor(col, row), 1, wxPENSTYLE_SOLID));
    dc.DrawLine(rect.x, rect.y + rect.height - 1, rect.x + rect.width, rect.y + rect.height - 1);
    // right border
    dc.SetPen(wxPen(renderer->verticalBorderColor(col, row), 1, wxPENSTYLE_SOLID));
    dc.DrawLine(rect.x + rect.width - 1, rect.y, rect.x + rect.width - 1, rect.y + rect.height - 1);

    // Text
    rect.x += 4;
    rect.width -= 8;
    rect.y += 1;

    const wxString& text = grid.GetCellValue(row, col);
    int hAlign, vAlign;
    attr.GetAlignment(&hAlign, &vAlign);
    grid.DrawTextRectangle(dc, text, rect, hAlign, vAlign);
}

} // namespace Datagrid
} // namespace Component
} // namespace Antares
