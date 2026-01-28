// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0


#include "wxgrid-renderer.h"
#include <wx/settings.h>
#include <wx/dc.h>

using namespace Yuni;

namespace Antares::Component::Datagrid
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

AntaresWxGridRenderer::AntaresWxGridRenderer():
    wxGridCellFloatRenderer(-1, 3),
    renderer(nullptr)
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
    {
        return;
    }
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

} // namespace Antares::Component::Datagrid
