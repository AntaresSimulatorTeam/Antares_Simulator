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

#include "dbgrid.h"
#include "component.h"
#include "renderer.h"
#include "../../../application/main.h"
#include <wx/dcclient.h>
#include <yuni/core/math.h>
#include "../../../application/wait.h"
#include "../../../application/study.h"
#include <wx/dc.h>
#include <wx/settings.h>
#include <ui/common/lock.h>

using namespace Yuni;

namespace Antares
{
namespace Component
{
namespace Datagrid
{
static const wxColour borderColor = wxColour(179, 179, 182);
static const wxColour borderDarkColor = wxColour(139, 139, 152);

// const wxColour gradientColStart = wxColour(229, 232, 233);
// const wxColour gradientColEnd   = wxColour(195, 203, 217);
enum
{
    plus = 120,
    moins = 90,
};
static const wxColour gradientColStart = wxColour(103 + plus, 111 + plus, 126 + plus);
static const wxColour gradientColEnd = wxColour(99 + moins, 107 + moins, 122 + moins);

static const wxColour gradientRowStart = wxColour(234, 237, 237);
static const wxColour gradientRowEnd = wxColour(216, 224, 238);

// The font re-used for each drawing
enum
{
    fontSize = 8,
};
static const wxFont font(wxFontInfo(fontSize).Family(wxFONTFAMILY_SWISS).FaceName("Tahoma"));

BEGIN_EVENT_TABLE(DBGrid, wxGrid)
EVT_GRID_SELECT_CELL(DBGrid::onGridSelectCell)
EVT_GRID_RANGE_SELECT(DBGrid::onGridRangeSelect)
EVT_PAINT(DBGrid::onDraw)
EVT_MENU(60001, DBGrid::evtOnResizeHeaders)
EVT_KEY_UP(DBGrid::onKeyUp)
EVT_ERASE_BACKGROUND(DBGrid::onEraseBackground)
EVT_SCROLLWIN(DBGrid::onScroll)
EVT_KILL_FOCUS(DBGrid::onGridLeave)
END_EVENT_TABLE()

DBGrid::DBGrid(Component* parent) :
 wxGrid(parent,
        wxID_ANY,
        wxDefaultPosition,
        wxDefaultSize,
        wxTAB_TRAVERSAL | wxWANTS_CHARS | wxBORDER_NONE),
 pParentComponent(parent),
 pAllowRefresh(true),
 pColorMappingRowLabels(true)
{
    DisableDragRowSize();
    SetColMinimalAcceptableWidth(20);
    SetRowMinimalAcceptableHeight(15);

    SetRowLabelAlignment(wxALIGN_LEFT, wxALIGN_CENTRE);
    SetDefaultCellAlignment(wxALIGN_RIGHT, wxALIGN_CENTRE);
    EnableDragGridSize(true);
    EnableDragColSize(true);
    EnableGridLines(false);
    SetGridLineColour(wxColour(220, 220, 220));

    // Dynamic event
    GetGridCornerLabelWindow()->Connect(wxEVT_PAINT,
                                        wxPaintEventHandler(DBGrid::evtCornerPaint),
                                        nullptr,
                                        GetGridCornerLabelWindow());
    GetGridColLabelWindow()->Connect(
      wxEVT_PAINT, wxPaintEventHandler(DBGrid::onDrawColLabels), nullptr, GetGridColLabelWindow());
    GetGridRowLabelWindow()->Connect(
      wxEVT_PAINT, wxPaintEventHandler(DBGrid::onDrawRowLabels), nullptr, GetGridRowLabelWindow());

    // Erase background
    GetGridCornerLabelWindow()->Connect(wxEVT_ERASE_BACKGROUND,
                                        wxEraseEventHandler(DBGrid::onEraseBackground));
    GetGridWindow()->Connect(wxEVT_ERASE_BACKGROUND,
                             wxEraseEventHandler(DBGrid::onEraseBackground));

    // These two events should not be used, otherwise some parts
    // of the panel will not be redrawn
    // GetGridColLabelWindow()->Connect(wxEVT_ERASE_BACKGROUND,
    // wxEraseEventHandler(DBGrid::onEraseBackground));
    // GetGridRowLabelWindow()->Connect(wxEVT_ERASE_BACKGROUND,
    // wxEraseEventHandler(DBGrid::onEraseBackground));
}

DBGrid::~DBGrid()
{
    // Remove any remaining reference
    auto* mainFrm = Forms::ApplWnd::Instance();
    if (mainFrm)
        mainFrm->disableGridOperatorIfGrid(this);
}

void DBGrid::onGridSelectCell(wxGridEvent& evt)
{
    assert(GetParent() && "invalid parent");

    auto* mainFrm = Forms::ApplWnd::Instance();
    if (mainFrm)
        mainFrm->gridOperatorSelectedCellsUpdateResult(this);
    pCurrentPosition.x = evt.GetCol();
    pCurrentPosition.y = evt.GetRow();

    auto* r = ((Component*)GetParent())->renderer();
    if (r)
        r->onSelectCell((unsigned)pCurrentPosition.x, (unsigned)pCurrentPosition.y);

    evt.Skip();
}

void DBGrid::onGridRangeSelect(wxGridRangeSelectEvent& evt)
{
    assert(GetGridWindow() && "invalid grid window");

    Forms::ApplWnd* mainFrm = Forms::ApplWnd::Instance();
    if (mainFrm)
        mainFrm->gridOperatorSelectedCellsUpdateResult(this);
    if (GetGridWindow())
        GetGridWindow()->SetFocus();
    evt.Skip();
}

void DBGrid::onGridLeave(wxFocusEvent& evt)
{
    auto* mainFrm = Forms::ApplWnd::Instance();
    if (mainFrm)
        mainFrm->gridOperatorSelectedCellsUpdateResult(nullptr);
    evt.Skip();
}

bool DBGrid::dataAreReady() const
{
    if (pAllowRefresh)
    {
        assert(GetParent() && "invalid parent");
        Renderer::IRenderer* r = ((Component*)GetParent())->renderer();
        return r || r->invalidate;
    }
    return false;
}

void DBGrid::onDraw(wxPaintEvent& evt)
{
    if (!dataAreReady())
    {
        // colors
        static const wxPen whitePen(wxColour(255, 255, 255), 1, wxPENSTYLE_SOLID);
        static const wxBrush whiteBrush(wxColour(255, 255, 255), wxBRUSHSTYLE_SOLID);

        // Entering into a WIP mode...
        WIP::Enter();

        wxPaintDC dc(this);
        const wxRect rect = GetRect();
        dc.SetPen(whitePen);
        dc.SetBrush(whiteBrush);
        dc.DrawRectangle(0, 0, rect.width, rect.height);

        Dispatcher::GUI::Post(this, &DBGrid::ensureDataAreLoadedDelayed);
        return;
    }
    evt.Skip();
}

void DBGrid::ensureDataAreLoaded()
{
    if (!pAllowRefresh)
        return;
    Component* parent = dynamic_cast<Component*>(GetParent());
    if (!parent)
        return;

    // avoid useless memory flush
    MemoryFlushLocker memflushlocker;

    Renderer::IRenderer* r = parent->renderer();
    if (r && (r->ensureDataAreLoaded() || r->invalidate))
    {
        r->invalidate = false;
        // Post an event to update the gui after the data are loaded

        Forms::ApplWnd* mainFrm = Forms::ApplWnd::Instance();
        if (mainFrm)
            mainFrm->disableGridOperatorIfGrid(this);

        assert(pAllowRefresh == true);
        parent->forceRefresh();
        if (GetTable())
            SetTable(GetTable(), false);
        Refresh();
    }
}

void DBGrid::ensureDataAreLoadedDelayed()
{
    // Note : WIP::Leave must be called once before leaving this routine
    ensureDataAreLoaded();
    WIP::Leave();
}

void DBGrid::onKeyUp(wxKeyEvent& evt)
{
    if (!GUIIsLock())
    {
        if (pParentComponent && evt.GetModifiers() == wxMOD_CONTROL)
        {
            const int c = evt.GetKeyCode();
            if (c == 'c' || c == 'C')
            {
                Dispatcher::GUI::Post(pParentComponent, &Component::copyToClipboard);
                return;
            }
            else
            {
                if (c == 'v' || c == 'V')
                    Dispatcher::GUI::Post(pParentComponent, &Component::pasteFromClipboard);
                // pParentComponent->pasteFromClipboard();
            }
        }
    }
    evt.Skip();
}

void DBGrid::copyToClipboard()
{
    if (pParentComponent)
        pParentComponent->copyToClipboard();
}

void DBGrid::copyAllToClipboard()
{
    if (pParentComponent)
        pParentComponent->copyAllToClipboard();
}

void DBGrid::pasteFromClipboard()
{
    if (pParentComponent)
        pParentComponent->pasteFromClipboard();
}

void DBGrid::resizeAllHeaders(bool nodelay)
{
    if (!nodelay)
    {
        // Post an event to update the gui after the data are loaded
        wxCommandEvent e(wxEVT_COMMAND_MENU_SELECTED, 60001);
        AddPendingEvent(e);
    }
    else
    {
        // avoid useless memory flush
        MemoryFlushLocker memflushlocker;

        Renderer::IRenderer* r = ((Component*)GetParent())->renderer();
        if (r && r->width() && r->height() > 0)
        {
            const int max = GetNumberCols();
            const int maxH = GetNumberRows();

            if (maxH > 0)
            {
                int rowLabelWidth = 40;
                int colLabelHeight = 5;

                if (GetGridRowLabelWindow())
                {
                    // SetRowLabelSize(wxGRID_AUTOSIZE);
                    //
                    // We should use the method `SetRowLabelSize()` instead but this method
                    // is really really slow, mainly because all rows are tested.
                    //
                    // There are two ways to optimize that :
                    // 1 - Compute the good size for visible grid only
                    //     All grids are resized without any exception and the operation
                    //     is quite useless. However, it may become difficult to know
                    //     what grid is visible or not
                    // 2 - Compute the good size with one or two rows
                    //     Most of the time we know the shape of each grid and the length
                    //     of a specific row is nearly the same than the best length for row labels.
                    //
                    wxString s;
                    wxString tmp;

                    if (maxH > 6000)
                    {
                        s = r->rowCaption(6100);
                    }
                    else
                    {
                        for (int i = 0; i < maxH; ++i)
                        {
                            tmp = r->rowCaption(i);
                            if (tmp.size() > s.size())
                                s = tmp;
                        }
                    }
                    wxClientDC dc(GetGridRowLabelWindow());
                    dc.SetFont(font);
                    wxSize p = dc.GetTextExtent(s);
                    if (p.GetWidth() > rowLabelWidth)
                        rowLabelWidth = p.GetWidth();
                }
                SetRowLabelSize(rowLabelWidth + 25);
                m_defaultRowHeight = 18;

                if (GetGridColLabelWindow())
                {
                    wxClientDC dc(GetGridColLabelWindow());
                    dc.SetFont(font);

                    for (int i = 0; i < max; ++i)
                    {
                        const int mode = r->columnWidthCustom(i);
                        if (mode > 0)
                        {
                            SetColMinimalWidth(i, mode);
                            SetColSize(i, mode);
                            continue;
                        }
                        if (mode < 0)
                        {
                            // From user data
                            wxString text;
                            r->hintForColumnWidth(i, text);
                            if (text.empty())
                            {
                                SetColMinimalWidth(i, 20);
                                SetColSize(i, 20);
                                continue;
                            }
                            else
                            {
                                wxSize p = dc.GetMultiLineTextExtent(text);
                                p.SetWidth(p.GetWidth() + 15);
                                if (p.GetHeight() > colLabelHeight)
                                    colLabelHeight = p.GetHeight();

                                if (p.GetWidth() < 70)
                                {
                                    SetColMinimalWidth(i, 70);
                                    SetColSize(i, 70);
                                }
                                else
                                {
                                    SetColMinimalWidth(i, p.GetWidth());
                                    SetColSize(i, p.GetWidth());
                                }
                            }

                            continue;
                        }

                        // Automatic mode
                        const wxString& text = r->columnCaption(i);
                        if (text.empty())
                        {
                            SetColMinimalWidth(i, 20);
                            SetColSize(i, 20);
                            continue;
                        }
                        else
                        {
                            wxSize p = dc.GetMultiLineTextExtent(text);
                            p.SetWidth(p.GetWidth() + 15);
                            if (p.GetHeight() > colLabelHeight)
                                colLabelHeight = p.GetHeight();

                            if (p.GetWidth() < 70)
                            {
                                SetColMinimalWidth(i, 70);
                                SetColSize(i, 70);
                            }
                            else
                            {
                                SetColMinimalWidth(i, p.GetWidth());
                                SetColSize(i, p.GetWidth());
                            }
                        }
                    }
                    colLabelHeight += 4;
                }

                // Height of the fixed-column headers
                SetColLabelSize(colLabelHeight + 2);
            }
        }
    }
}

void DBGrid::evtOnResizeHeaders(wxCommandEvent&)
{
    resizeAllHeaders(true);
}

void DBGrid::DrawColLabel(wxDC& dc, int col, uint& offset)
{
    if (GetColWidth(col) <= 0 || m_colLabelHeight <= 0 || col < 0)
        return;

    wxRect rect;
    const int colLeft = GetColLeft(col);
    rect.x = colLeft;
    rect.y = 0;
    rect.width = GetColWidth(col);
    rect.height = m_colLabelHeight;
    {
        const uint m = rect.x + rect.width;
        if (m > offset)
            offset = m;
    }

    dc.GradientFillLinear(rect, gradientColStart, gradientColEnd, wxSOUTH);

    int colRight = GetColRight(col);

    // The right border
    dc.SetPen(wxPen(borderColor, 1, wxPENSTYLE_SOLID));
    dc.DrawLine(colLeft, 0, colRight, 0);
    dc.SetPen(wxPen(borderDarkColor, 1, wxPENSTYLE_SOLID));
    dc.DrawLine(colLeft, m_colLabelHeight - 1, colRight, m_colLabelHeight - 1);

    // Separator
    dc.SetPen(wxPen(gradientColEnd, 1, wxPENSTYLE_SOLID));
    if (col < GetNumberCols() - 1)
        dc.DrawLine(colRight - 1, 4, colRight - 1, m_colLabelHeight - 4);
    else
        dc.DrawLine(colRight - 1, 0, colRight - 1, m_colLabelHeight - 1);

    // text
    dc.SetBackgroundMode(wxTRANSPARENT);
    dc.SetTextForeground(wxColour(205, 205, 205));
    dc.SetFont(font);
    int hAlign, vAlign, orient;
    GetColLabelAlignment(&hAlign, &vAlign);
    orient = GetColLabelTextOrientation();

    rect.y += 1;
    dc.SetTextForeground(wxColour(240, 240, 240));
    DrawTextRectangle(dc, GetColLabelValue(col), rect, hAlign, vAlign, orient);
    rect.x -= 1;
    rect.y -= 1;
    dc.SetTextForeground(wxColour(30, 30, 30));
    DrawTextRectangle(dc, GetColLabelValue(col), rect, hAlign, vAlign, orient);
}

void DBGrid::DrawRowLabel(wxDC& dc, int row, uint& offset)
{
    if (GetRowHeight(row) <= 0 || m_rowLabelWidth <= 0 || row < 0)
        return;

    wxRect rect;
    rect.x = 0;
    rect.y = GetRowTop(row);
    rect.width = m_rowLabelWidth;
    rect.height = GetRowHeight(row);

    {
        const uint m = rect.y + rect.height;
        if (m > offset)
            offset = m;
    }

    if (pColorMappingRowLabels)
        dc.GradientFillLinear(rect, gradientRowStart, gradientRowEnd, wxEAST);

    // borders
    {
        const int rowTop = GetRowTop(row);
        const int rowBottom = GetRowBottom(row) - 1;

        dc.SetPen(wxPen(borderColor, 1, wxPENSTYLE_SOLID));
        dc.DrawLine(m_rowLabelWidth - 1, rowTop, m_rowLabelWidth - 1, rowBottom);
        dc.DrawLine(0, rowTop, 0, rowBottom);
        dc.DrawLine(0, rowBottom, m_rowLabelWidth, rowBottom);
    }

    // text
    rect.x += 10;
    dc.SetBackgroundMode(wxTRANSPARENT);
    dc.SetFont(font);
    int hAlign, vAlign;
    GetRowLabelAlignment(&hAlign, &vAlign);

    const wxString& text = GetRowLabelValue(row);

    // dc.SetTextForeground(wxColour(205, 205, 205));
    // DrawTextRectangle(dc, text, rect, hAlign, vAlign);
    //
    rect.y += 1;
    dc.SetTextForeground(wxColour(245, 245, 245));
    DrawTextRectangle(dc, text, rect, hAlign, vAlign);

    rect.x -= 1;
    rect.y -= 1;
    dc.SetTextForeground(wxColour(20, 20, 20));
    DrawTextRectangle(dc, text, rect, hAlign, vAlign);
}

void DBGrid::DrawCellHighlight(wxDC& dc, const wxGridCellAttr* attr)
{
    // don't show highlight when the grid doesn't have focus
    if (wxWindow::FindFocus() != GetGridWindow())
        return;

    const int row = m_currentCellCoords.GetRow();
    const int col = m_currentCellCoords.GetCol();

    if (GetColWidth(col) <= 0 || GetRowHeight(row) <= 0)
        return;

    if (!attr->IsReadOnly() && !IsInSelection(row, col))
    {
        wxRect rect = CellToRect(row, col);
        const wxColour& txtColor = wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHT);
        dc.SetPen(wxPen(txtColor, 1, wxPENSTYLE_SOLID));
        dc.SetBrush(*wxTRANSPARENT_BRUSH);
        dc.DrawRectangle(rect);
    }
}

void DBGrid::evtCornerPaint(wxPaintEvent&)
{
    wxPaintDC dc(this);

    int client_height = 0;
    int client_width = 0;
    GetClientSize(&client_width, &client_height);

    dc.GradientFillLinear(
      wxRect(0, 0, client_width, client_height), gradientColStart, gradientColEnd, wxSOUTH);

    dc.SetPen(wxPen(borderColor, 1, wxPENSTYLE_SOLID));
    dc.DrawLine(client_width - 1, client_height - 1, client_width - 1, 0);
    dc.DrawLine(0, 0, client_width, 0);
    dc.DrawLine(0, 0, 0, client_height);
    dc.SetPen(wxPen(borderDarkColor, 1, wxPENSTYLE_SOLID));
    dc.DrawLine(client_width - 1, client_height - 1, 0, client_height - 1);

    int size = 6;
    dc.SetPen(wxPen(wxColour(134, 134, 137), 1, wxPENSTYLE_SOLID));
    dc.SetBrush(wxBrush(wxColour(144, 144, 147), wxBRUSHSTYLE_SOLID));
    wxPoint pts[3];
    pts[0].x = client_width - 4 - size;
    pts[0].y = client_height - 4;
    pts[1].x = client_width - 4;
    pts[1].y = client_height - 4;
    pts[2].x = client_width - 4;
    pts[2].y = client_height - 4 - size;

    dc.DrawPolygon(3, pts);
}

void DBGrid::onDrawColLabels(wxPaintEvent&)
{
    // alias to the parent window
    DBGrid* parent = dynamic_cast<DBGrid*>(GetParent());
    if (!parent)
        return;

    wxPaintDC dc(this);
    // NO - don't do this because it will set both the x and y origin
    // coords to match the parent scrolled window and we just want to
    // set the y coord  - MB
    // m_owner->PrepareDC( dc );
    {
        int x, y;
        parent->CalcUnscrolledPosition(0, 0, &x, &y);
        const wxPoint pt = dc.GetDeviceOrigin();
        if (GetLayoutDirection() == wxLayout_RightToLeft)
            dc.SetDeviceOrigin(pt.x + x, pt.y);
        else
            dc.SetDeviceOrigin(pt.x - x, pt.y);
    }
    // Drawing each column
    uint offset = 0;
    {
        const wxArrayInt cols = parent->CalcColLabelsExposed(GetUpdateRegion());
        uint count = (uint)cols.GetCount();
        for (uint i = 0; i != count; ++i)
            parent->DrawColLabel(dc, cols[i], offset);
    }

    const wxRect rect = GetRect();
    dc.SetPen(wxPen(GetBackgroundColour(), 1, wxPENSTYLE_SOLID));
    dc.SetBrush(wxBrush(GetBackgroundColour(), wxBRUSHSTYLE_SOLID));
    if (static_cast<int>(offset) < rect.width)
        dc.DrawRectangle(offset, 0, rect.width - offset, rect.height);
}

void DBGrid::onDrawRowLabels(wxPaintEvent&)
{
    // alias to the parent window
    DBGrid* parent = dynamic_cast<DBGrid*>(GetParent());
    if (!parent)
        return;

    wxPaintDC dc(this);
    // NO - don't do this because it will set both the x and y origin
    // coords to match the parent scrolled window and we just want to
    // set the y coord  - MB
    // m_owner->PrepareDC( dc );
    {
        int y;
        parent->CalcUnscrolledPosition(0, 0, nullptr, &y);
        const wxPoint pt = dc.GetDeviceOrigin();
        dc.SetDeviceOrigin(pt.x, pt.y - y);
    }
    // Drawing each column
    uint offset = 0;
    {
        const wxArrayInt cols = parent->CalcRowLabelsExposed(GetUpdateRegion());
        uint count = (uint)cols.GetCount();
        for (uint i = 0; i != count; ++i)
            parent->DrawRowLabel(dc, cols[i], offset);
    }

    const wxColour& background = GetBackgroundColour();
    dc.SetPen(wxPen(background, 1, wxPENSTYLE_SOLID));
    dc.SetBrush(wxBrush(background, wxBRUSHSTYLE_SOLID));
    const wxRect rect = GetRect();
    if (static_cast<int>(offset) < rect.height)
        dc.DrawRectangle(0, offset, rect.width, rect.height - offset);
}

void DBGrid::onScroll(wxScrollWinEvent& evt)
{
    Renderer::IRenderer* r = ((Component*)GetParent())->renderer();
    if (r)
        r->onScroll();
    evt.Skip();
}

} // namespace Datagrid
} // namespace Component
} // namespace Antares
