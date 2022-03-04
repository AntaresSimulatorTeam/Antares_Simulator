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

#include <antares/antares.h>
#include "notebook.h"
#include <wx/dcbuffer.h>
#include <wx/sizer.h>
#include <wx/font.h>
#include "../../components/map/settings.h"
#include <antares/logs.h>
#include "../../../application/wait.h"
#include <ui/common/lock.h>
#include <wx/wupdlock.h>
#include "../refresh.h"

#define NODE_DRAW_COLOR_VARIATION_LIGHT 10
#define NODE_DRAW_COLOR_VARIATION_DARK 5

using namespace Yuni;

namespace Antares
{
namespace Component
{
BEGIN_EVENT_TABLE(Notebook::Tabs, Panel)
EVT_PAINT(Notebook::Tabs::onDraw)
END_EVENT_TABLE()

Notebook::Notebook(wxWindow* parent, Notebook::Orientation orientation) :
 Panel(parent),
 pOrientation(orientation),
 pTabs(nullptr),
 pTabsVisible(true),
 pAlwaysDisplayTab(false),
 pDisplayTitle(true),
 pSizerForPages(nullptr),
 pTopSizer(nullptr),
 pLeftSizer(nullptr),
 pLastSelected(nullptr),
 pTheme(themeDefault),
 pAlignment(alignRight)
{
    pOriginalColor = GetBackgroundColour();
    pOriginalColorDark.Set((unsigned char)Math::MinMax<int>(pOriginalColor.Red() - 70, 0, 255),
                           (unsigned char)Math::MinMax<int>(pOriginalColor.Green() - 70, 0, 255),
                           (unsigned char)Math::MinMax<int>(pOriginalColor.Blue() - 70, 0, 255));

    SetBackgroundStyle(wxBG_STYLE_CUSTOM); // Mandatory on both GTK & Windows
    SetExtraStyle(wxWS_EX_BLOCK_EVENTS);

    pTabs = new Tabs(this, *this);
    wxBoxSizer* sizer = new wxBoxSizer((orientation == orLeft ? wxHORIZONTAL : wxVERTICAL));
    SetSizer(sizer);
    sizer->Add(pTabs, 0, wxALL | wxEXPAND);

    switch (orientation)
    {
    case orLeft:
    {
        SetSize(130, 18);
        sizer->SetItemMinSize(pTabs, 130, 18);
        break;
    }
    case orTop:
    {
        sizer->SetItemMinSize(pTabs, -1, 24);
        break;
    }
    }

    pLeftSizer = new wxBoxSizer(wxHORIZONTAL);
    pTopSizer = new wxBoxSizer(wxVERTICAL);
    pLeftSizer->Add(pTopSizer, 1, wxALL | wxEXPAND);
    sizer->Add(pLeftSizer, 1, wxALL | wxEXPAND);
    pSizerForPages = pTopSizer;
}

Notebook::~Notebook()
{
    clear();

    // To avoid corrupt vtable in some rare cases / misuses
    // (when children try to access to this object for example),
    // we should destroy all children as soon as possible.
    wxSizer* sizer = GetSizer();
    if (sizer)
        sizer->Clear(true);
}

Notebook::Tabs::Tabs(wxWindow* parent, Notebook& notebook) :
 Panel(parent), pNotebook(notebook), pCachedSize(0, 0), pMaxFound(0)
{
    SetBackgroundStyle(wxBG_STYLE_CUSTOM); // Needed by Windows
    pCachedSize.x = 150;
    pCachedSize.y = 18;
    SetSizer(new wxBoxSizer(wxHORIZONTAL));
}

void Notebook::Tabs::onEraseBackground(wxEraseEvent&)
{
    // Do nothing to prevent flickering
}

void Notebook::Tabs::onDraw(wxPaintEvent&)
{
    // The font re-used for each drawing
    enum
    {
        fontSize = 8,
    };
    static const wxFont font(wxFontInfo(fontSize).Family(wxFONTFAMILY_SWISS).FaceName("Tahoma"));

    // The DC
    wxAutoBufferedPaintDC dc(this);
    if (dc.IsOk())
    {
        // Shifts the device origin so we don't have to worry
        // about the current scroll position ourselves
        PrepareDC(dc);
        if (dc.IsOk())
        {
            dc.SetFont(font);
            pRect = this->GetClientSize();

            switch (pNotebook.pOrientation)
            {
            case Notebook::orLeft:
                drawOrientationLeft(dc);
                break;
            case Notebook::orTop:
                drawOrientationTop(dc);
                break;
            }
        }
    }
}

void Notebook::Tabs::drawOrientationLeft(wxDC& dc)
{
    switch (pNotebook.pTheme)
    {
    case themeDefault:
    {
        dc.SetPen(wxPen(wxColour(100, 100, 100)));
        dc.SetBrush(wxBrush(wxColour(100, 100, 100), wxBRUSHSTYLE_SOLID));

        dc.DrawRectangle(0, 0, pRect.width, pRect.height);
        dc.SetPen(wxPen(wxColour(110, 110, 110), 1, wxPENSTYLE_SOLID));
        dc.DrawLine(0, 0, pRect.width, 0);
        dc.SetPen(wxPen(wxColour(11, 11, 11), 1, wxPENSTYLE_SOLID));
        dc.DrawLine(pRect.width - 1, 0, pRect.width - 1, pRect.GetHeight());

        break;
    }
    case themeLight:
    {
        wxColour bg;
        wxColour bgDark;
        bg.Set((unsigned char)Math::MinMax<int>(pNotebook.pOriginalColor.Red() + 40, 0, 255),
               (unsigned char)Math::MinMax<int>(pNotebook.pOriginalColor.Green() + 40, 0, 255),
               (unsigned char)Math::MinMax<int>(pNotebook.pOriginalColor.Blue() + 40, 0, 255));
        bgDark.Set(
          (unsigned char)Math::MinMax<int>(pNotebook.pOriginalColorDark.Red() + 20, 0, 255),
          (unsigned char)Math::MinMax<int>(pNotebook.pOriginalColorDark.Green() + 20, 0, 255),
          (unsigned char)Math::MinMax<int>(pNotebook.pOriginalColorDark.Blue() + 20, 0, 255));

        dc.SetPen(wxPen(pNotebook.pOriginalColor, 1, wxPENSTYLE_SOLID));
        dc.SetBrush(wxBrush(pNotebook.pOriginalColor, wxBRUSHSTYLE_SOLID));
        dc.SetPen(wxPen(bg, 1, wxPENSTYLE_SOLID));
        dc.SetBrush(wxBrush(bg, wxBRUSHSTYLE_SOLID));

        dc.DrawRectangle(0, 0, pRect.width, pRect.height);
        dc.SetPen(wxPen(wxColour(40, 40, 40), 1, wxPENSTYLE_SOLID));
        dc.DrawLine(0, 0, pRect.width, 0);

        dc.GradientFillLinear(
          wxRect(pRect.x + pRect.width - 6, 0, 6, pRect.height), bg, bgDark, wxEAST);

        dc.SetPen(wxPen(pNotebook.pOriginalColorDark, 1, wxPENSTYLE_SOLID));
        dc.DrawLine(pRect.width - 1, 0, pRect.width - 1, pRect.GetHeight());
        dc.DrawLine(0, 0, pRect.width - 1, 0);
        dc.DrawLine(0, pRect.height - 1, pRect.width - 1, pRect.height - 1);
        break;
    }
    }

    pMaxFound = 0;
    int pos = 20;

    if (pNotebook.pTabsVisible && (pNotebook.pAlwaysDisplayTab || pNotebook.pPages.size() > 1))
    {
        const Pages::const_iterator end = pNotebook.pPages.end();
        for (Pages::const_iterator i = pNotebook.pPages.begin(); i != end; ++i)
            drawItem(dc, *i, pos);
    }

    if (pMaxFound != pCachedSize.x)
    {
        pCachedSize.x = pMaxFound;
        Dispatcher::GUI::Post(this, &Tabs::onForceRefresh);
    }
}

void Notebook::forceRefresh()
{
    Dispatcher::GUI::Post(this, &Notebook::onForceRefresh, 20 /*ms*/);
}

void Notebook::onForceRefresh()
{
    // Force the send of a resize event
    // To prevent against unwanted behavior.......

    pSizerForPages->Layout();
    GetSizer()->Layout();
    if (pLastSelected && pLastSelected->control())
    {
        wxWindow* wnd = pLastSelected->control();
        wxSize s = wnd->GetSize();
        if (s.GetWidth() || s.GetHeight())
        {
            wxEvtHandler* evtHandler = wnd->GetEventHandler();
            if (evtHandler)
            {
                wxSizeEvent e(s);
                evtHandler->ProcessEvent(e);
            }
            wnd->SetFocusFromKbd();
            Refresh();
            RefreshAllControls(wnd);
        }
    }
}

void Notebook::Tabs::onForceRefresh()
{
    if (GetParent() && GetParent()->GetSizer())
    {
        GetParent()->GetSizer()->SetItemMinSize(this, pCachedSize.x, pCachedSize.y);
        GetParent()->GetSizer()->Layout();
    }
    Dispatcher::GUI::Refresh(this);
}

void Notebook::Tabs::drawOrientationTop(wxDC& dc)
{
    wxColour ori;
    switch (pNotebook.pTheme)
    {
    case themeDefault:
        ori.Set(80, 80, 80);
        break;
    case themeLight:
        ori.Set(101, 109, 123);
        break;
    }

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

    if (pNotebook.pDisplayTitle && pNotebook.pTabsVisible)
    {
        if (pNotebook.pAlwaysDisplayTab || pNotebook.pPages.size() > 1)
        {
            switch (pNotebook.pAlignment)
            {
            case alignLeft:
            {
                int pos = 15;
                if (!pNotebook.pCaption.empty())
                {
                    const wxSize size = dc.GetTextExtent(pNotebook.pCaption);
                    dc.SetTextForeground(wxColour(50, 50, 50));
                    dc.DrawText(
                      pNotebook.pCaption, 31, 1 + (pRect.height >> 1) - (size.GetHeight() >> 1));
                    dc.SetTextForeground(wxColour(255, 166, 25));
                    dc.DrawText(
                      pNotebook.pCaption, 30, (pRect.height >> 1) - (size.GetHeight() >> 1));
                    pos = 30 + size.GetWidth() + 10;
                }

                const Pages::const_iterator end = pNotebook.pPages.end();
                for (Pages::const_iterator i = pNotebook.pPages.begin(); i != end; ++i)
                {
                    drawItemTop(dc, *i, pos, Notebook::alignLeft);
                }
                break;
            }
            case alignRight:
            {
                int pos = pRect.GetWidth() - 15;
                const Pages::const_reverse_iterator end = pNotebook.pPages.rend();
                for (Pages::const_reverse_iterator i = pNotebook.pPages.rbegin(); i != end; ++i)
                {
                    drawItemTop(dc, *i, pos, Notebook::alignRight);
                }
                if (!pNotebook.pCacheTitle.IsEmpty())
                {
                    const wxSize size = dc.GetTextExtent(pNotebook.pCacheTitle);
                    dc.SetTextForeground(wxColour(50, 50, 50));
                    dc.DrawText(
                      pNotebook.pCacheTitle, 31, 1 + (pRect.height >> 1) - (size.GetHeight() >> 1));
                    dc.SetTextForeground(wxColour(255, 166, 25));
                    dc.DrawText(
                      pNotebook.pCacheTitle, 30, (pRect.height >> 1) - (size.GetHeight() >> 1));
                }
                break;
            }
            }
        }
    }
    else
    {
        if (pNotebook.pAlwaysDisplayTab || pNotebook.pPages.size() > 1)
        {
            switch (pNotebook.pAlignment)
            {
            case alignLeft:
            {
                int pos = 15;
                const Pages::const_iterator end = pNotebook.pPages.end();
                for (Pages::const_iterator i = pNotebook.pPages.begin(); i != end; ++i)
                    drawItemTop(dc, *i, pos, alignLeft);
                break;
            }
            case alignRight:
            {
                int pos = pRect.GetWidth() - 15;
                const Pages::const_reverse_iterator end = pNotebook.pPages.rend();
                for (Pages::const_reverse_iterator i = pNotebook.pPages.rbegin(); i != end; ++i)
                    drawItemTop(dc, *i, pos, alignRight);
                break;
            }
            }
        }
    }
}

void Notebook::Tabs::drawItemTop(wxDC& dc, Page* page, int& pos, Notebook::Alignment align)
{
    static const wxColour colBlack(0, 0, 0);
    static const wxColour colGray(100, 100, 100);

    if (!page || !page->visible())
    {
        return;
    }

    if (page->caption().empty())
    {
        switch (align)
        {
        case alignRight:
            pos -= 15;
            break;
        case alignLeft:
            pos += 15;
            break;
        }
    }
    else
    {
        const wxSize textExtent = dc.GetTextExtent(page->caption());
        const wxRect rect(pos - ((align == alignRight) ? (textExtent.GetWidth() + 8 * 2) : 0),
                          pRect.y + 3,
                          textExtent.GetWidth() + 8 * 2,
                          pRect.height - 6);

        dc.SetBrush(wxBrush(colGray, wxBRUSHSTYLE_TRANSPARENT));
        if (!page->selected())
        {
            dc.SetTextForeground(wxColour(50, 50, 50));
            dc.DrawText(page->caption(),
                        rect.x + 8,
                        pRect.y + pRect.height / 2 - textExtent.GetHeight() / 2 + 1);
            dc.SetTextForeground(wxColour(230, 230, 230));
            dc.DrawText(
              page->caption(), rect.x + 7, pRect.y + pRect.height / 2 - textExtent.GetHeight() / 2);
        }
        else
        {
            const wxColour pColor(Map::Settings::background);
            const wxColour gColor((unsigned char)Math::MinMax<int>(pColor.Red() - 50, 0, 255),
                                  (unsigned char)Math::MinMax<int>(pColor.Green() - 50, 0, 255),
                                  (unsigned char)Math::MinMax<int>(pColor.Blue() - 50, 0, 255));

            dc.GradientFillLinear(wxRect(rect.x, rect.y, rect.width, 4), colGray, gColor, wxSOUTH);
            dc.GradientFillLinear(wxRect(rect.x, rect.y + 4, rect.width, rect.height - 5),
                                  gColor,
                                  Map::Settings::background,
                                  wxSOUTH);

            switch (pNotebook.theme())
            {
            case themeLight:
                dc.SetPen(wxPen(wxColour(85, 85, 85), 1, wxPENSTYLE_SOLID));
                break;
            default:
                dc.SetPen(wxPen(wxColour(0, 0, 0), 1, wxPENSTYLE_SOLID));
            }
            dc.SetBrush(*wxTRANSPARENT_BRUSH);
            dc.DrawRectangle(rect.x, rect.y, rect.width, rect.height);

            dc.SetTextForeground(colBlack);
            dc.DrawText(
              page->caption(), rect.x + 8, pRect.y + pRect.height / 2 - textExtent.GetHeight() / 2);
        }

        switch (align)
        {
        case alignRight:
            pos -= rect.width - 1;
            break;
        case alignLeft:
            pos += rect.width - 1;
            break;
        }

        page->pBoundingBox = rect;
    }
}

void Notebook::Tabs::drawItemOnCanvasNotSelected(Page* page,
                                                 wxDC& dc,
                                                 const int pos,
                                                 const int h,
                                                 const wxSize& textExtent)
{
    const int y = pos + (h >> 1) - (textExtent.GetHeight() >> 1) + 1;
    switch (pNotebook.pTheme)
    {
    case themeDefault:
    {
        dc.SetTextForeground(wxColour(0, 0, 0));
        dc.DrawText(page->caption(), pRect.width - textExtent.GetWidth() - 14, y + 1);
        dc.DrawText(page->caption(), pRect.width - textExtent.GetWidth() - 15, y + 1);
        dc.SetTextForeground(wxColour(230, 230, 240));
        dc.DrawText(page->caption(), pRect.width - textExtent.GetWidth() - 15, y);
        break;
    }
    case themeLight:
    {
        dc.SetTextForeground(wxColour(255, 255, 255));
        dc.DrawText(page->caption(), pRect.width - textExtent.GetWidth() - 14, y);
        dc.SetTextForeground(wxColour(60, 60, 60));
        dc.DrawText(page->caption(), pRect.width - textExtent.GetWidth() - 15, y - 1);
        break;
    }
    }
}

void Notebook::Tabs::drawItemOnCanvasSelected(Page* page,
                                              wxDC& dc,
                                              const int pos,
                                              const int h,
                                              const wxSize& textExtent)
{
    switch (pNotebook.pTheme)
    {
    case themeDefault:
    {
        dc.SetBrush(wxBrush(Map::Settings::background, wxBRUSHSTYLE_SOLID));
        dc.SetPen(wxPen(wxColour(80, 80, 80), 1, wxPENSTYLE_SOLID));
        dc.DrawRectangle(10, pos, pRect.width - 10, h);

        const wxColour pColor(Map::Settings::background);
        const wxColour gColor((unsigned char)Math::MinMax<int>(pColor.Red() - 50, 0, 255),
                              (unsigned char)Math::MinMax<int>(pColor.Green() - 50, 0, 255),
                              (unsigned char)Math::MinMax<int>(pColor.Blue() - 50, 0, 255));

        dc.GradientFillLinear(
          wxRect(11, pos + 1, pRect.width - 11, 4), wxColor(145, 145, 145), gColor, wxSOUTH);

        dc.GradientFillLinear(
          wxRect(11, pos + 5, pRect.width - 11, h - 5), gColor, Map::Settings::background, wxSOUTH);

        dc.SetTextForeground(wxColour(0, 0, 0));

        size_t size = page->subPages.size();
        size_t subTabSize = h / (size + 1);
        if (size == 0)
        {
            dc.DrawText(page->caption(),
                        pRect.width - textExtent.GetWidth() - 15,
                        pos + (h >> 1) - (textExtent.GetHeight() >> 1));

            pPts[0].x = pRect.width - 6;
            pPts[0].y = pos + (h >> 1) + 1;
            pPts[1].x = pPts[0].x - 3;
            pPts[1].y = pPts[0].y - 3;
            pPts[2].x = pPts[0].x - 3;
            pPts[2].y = pPts[0].y + 3;
        }
        else
        {
            dc.DrawText(
              page->caption(), 15, pos + (subTabSize >> 1) - (textExtent.GetHeight() >> 1));
            for (size_t i = 0; i < size; i++)
            {
                wxSize subpageTextExtent = dc.GetTextExtent(page->subPages[i]->caption());

                wxRect rect(11,
                            pos + subTabSize * (i + 1),
                            pRect.width - 15 - 11,
                            subpageTextExtent.GetHeight());
                dc.SetBrush(wxBrush(pNotebook.pOriginalColorDark, wxBRUSHSTYLE_SOLID));
                dc.SetPen(wxPen(pNotebook.pOriginalColorDark, 1, wxPENSTYLE_SOLID));
                dc.DrawRectangle(rect);

                page->subPages[i]->pBoundingBox = rect;

                dc.SetTextForeground(wxColour(255, 255, 255));
                dc.DrawText(page->subPages[i]->caption(),
                            pRect.width - subpageTextExtent.GetWidth() - 15,
                            pos + subTabSize * (i + 1));

                if (page->subPages[i]->selected())
                {
                    pPts[0].x = pRect.width - 6;
                    pPts[0].y
                      = pos + subTabSize * (i + 1) + (subpageTextExtent.GetHeight() >> 1) + 1;
                    pPts[1].x = pPts[0].x - 3;
                    pPts[1].y = pPts[0].y - 3;
                    pPts[2].x = pPts[0].x - 3;
                    pPts[2].y = pPts[0].y + 3;
                }
            }
        }

        dc.SetPen(wxPen(wxColour(130, 130, 130)));
        dc.DrawLine(pRect.width - 1, pos, pRect.width - 1, pos + h);

        dc.SetPen(wxPen(wxColour(183, 79, 31)));
        dc.SetBrush(wxBrush(wxColour(255, 110, 43)));
        dc.DrawPolygon(3, pPts);
        break;
    }
    case themeLight:
    {
        wxColour bg;
        wxColour bgDark;
        bg.Set((unsigned char)Math::MinMax<int>(pNotebook.pOriginalColor.Red() + 40, 0, 255),
               (unsigned char)Math::MinMax<int>(pNotebook.pOriginalColor.Green() + 40, 0, 255),
               (unsigned char)Math::MinMax<int>(pNotebook.pOriginalColor.Blue() + 40, 0, 255));
        bgDark.Set(
          (unsigned char)Math::MinMax<int>(pNotebook.pOriginalColorDark.Red() + 20, 0, 255),
          (unsigned char)Math::MinMax<int>(pNotebook.pOriginalColorDark.Green() + 20, 0, 255),
          (unsigned char)Math::MinMax<int>(pNotebook.pOriginalColorDark.Blue() + 20, 0, 255));

        wxColour bgDark2;
        bgDark2.Set(
          (unsigned char)Math::MinMax<int>(pNotebook.pOriginalColorDark.Red() - 40, 0, 255),
          (unsigned char)Math::MinMax<int>(pNotebook.pOriginalColorDark.Green() - 40, 0, 255),
          (unsigned char)Math::MinMax<int>(pNotebook.pOriginalColorDark.Blue() - 40, 0, 255));

        dc.SetBrush(wxBrush(pNotebook.pOriginalColorDark, wxBRUSHSTYLE_SOLID));
        dc.SetPen(wxPen(pNotebook.pOriginalColorDark, 1, wxPENSTYLE_SOLID));
        dc.DrawRectangle(15, pos, pRect.width - 15, h);

        dc.GradientFillLinear(
          wxRect(20, pos, pRect.width - 5, 1), pNotebook.pOriginalColorDark, bgDark2, wxEAST);
        dc.GradientFillLinear(wxRect(20, pos + h - 1, pRect.width - 5, 1),
                              pNotebook.pOriginalColorDark,
                              bgDark2,
                              wxEAST);

        dc.GradientFillLinear(wxRect(10, pos, 5, h), bg, pNotebook.pOriginalColorDark, wxEAST);

        dc.SetTextForeground(wxColour(255, 255, 255));
        dc.DrawText(page->caption(),
                    pRect.width - textExtent.GetWidth() - 15,
                    pos - 1 + (h >> 1) - (textExtent.GetHeight() >> 1) + 1);

        break;
    }
    }
}

void Notebook::Tabs::drawItem(wxDC& dc, Page* page, int& pos)
{
    if (!page || !page->visible())
    {
        return;
    }

    int h = (pNotebook.pTheme == themeDefault) ? 22 : 18;
    if (!(page->caption()).IsEmpty())
    {
        const wxSize textExtent = dc.GetTextExtent(page->caption());
        if (!page->selected())
            drawItemOnCanvasNotSelected(page, dc, pos, h, textExtent);
        else
        {
            if (page->subPages.size())
                for (size_t i = 0; i < page->subPages.size(); i++)
                    h += h >> 1;

            drawItemOnCanvasSelected(page, dc, pos, h, textExtent);
        }

        const int dreamSize = textExtent.GetWidth() + 45;
        if (pMaxFound < dreamSize)
            pMaxFound = dreamSize;
        page->pBoundingBox.x = 0;
        page->pBoundingBox.y = pos;
        page->pBoundingBox.width = pRect.width;
        page->pBoundingBox.height = h;
        pos += h;
    }
    else
        pos += h - 5;
}

void Notebook::clear()
{
    if (!pPages.empty())
    {
        pLastSelected = nullptr;
        {
            Pages copy(pPages);
            pPages.clear();
            const Pages::iterator end = copy.end();
            for (Pages::iterator i = copy.begin(); i != end; ++i)
                delete *i;
        }
    }
}

void Notebook::addCommonControl(Toolbox::InputSelector::AInput* input, const int border)
{
    this->addCommonControl(input, border, input->recommendedSize());
}

void Notebook::addCommonControl(wxWindow* ctrnl, const int border, const wxPoint& recommendedSize)
{
    if (ctrnl)
    {
        pLeftSizer->Prepend(ctrnl, 0, wxALL | wxEXPAND, border);
        if (recommendedSize.x && recommendedSize.y)
            pLeftSizer->SetItemMinSize(ctrnl, recommendedSize.x, recommendedSize.y);
        pExtraControls.push_back(ctrnl);
    }
}

void Notebook::addCommonControlTop(Toolbox::InputSelector::AInput* input, const int border)
{
    this->addCommonControlTop(input, border, input->recommendedSize());
}

void Notebook::addCommonControlTop(wxWindow* ctrnl,
                                   const int border,
                                   const wxPoint& recommendedSize)
{
    if (ctrnl)
    {
        pTopSizer->Prepend(ctrnl, 0, wxALL | wxEXPAND, border);
        pTopSizer->SetItemMinSize(ctrnl, recommendedSize.x, recommendedSize.y);
        pExtraControls.push_back(ctrnl);
    }
}

Notebook::Page::Page(Notebook& notebook) :
 pNotebook(notebook),
 pControl(nullptr),
 pSelected(false),
 pVisible(true),
 pDisplayExtraControls(true)
{
    pNotebook.pPages.push_back(this);
}

Notebook::Page::Page(Notebook& notebook, wxWindow* ctnrl, const wxString& caption) :
 pNotebook(notebook),
 pControl(ctnrl),
 pCaption(caption),
 pSelected(false),
 pVisible(true),
 pDisplayExtraControls(true)
{
    pNotebook.pPages.push_back(this);
    if (pControl)
    {
        pNotebook.pSizerForPages->Add(pControl, 1, wxALL | wxEXPAND);
        pNotebook.pSizerForPages->Show(pControl, false, false);
    }
    assert(!ctnrl || ctnrl->GetParent() == dynamic_cast<wxWindow*>(&notebook));
}

Notebook::Page::Page(Notebook& notebook,
                     wxWindow* ctnrl,
                     const wxString& name,
                     const wxString& caption) :
 pNotebook(notebook),
 pControl(ctnrl),
 pName(name),
 pCaption(caption),
 pSelected(false),
 pVisible(true),
 pDisplayExtraControls(true)
{
    pNotebook.pPages.push_back(this);

    if (pControl)
    {
        pNotebook.pSizerForPages->Add(pControl, 1, wxALL | wxEXPAND);
        pNotebook.pSizerForPages->Show(pControl, false, false);
    }

    assert(ctnrl);
    assert(ctnrl->GetParent() == dynamic_cast<wxWindow*>(&notebook));
}

Notebook::Page::~Page()
{
    if (pNotebook.pLastSelected == this)
        pNotebook.pLastSelected = nullptr;

    // Remove the page from the parent
    auto end = pNotebook.pPages.end();
    for (auto i = pNotebook.pPages.begin(); i != end; ++i)
    {
        if (*i == this)
        {
            pNotebook.pPages.erase(i);
            break;
        }
    }

    // Delete all controls
    if (pControl && pNotebook.pSizerForPages)
    {
        pNotebook.pSizerForPages->Detach(pControl);
        auto* sizer = pNotebook.GetSizer();
        if (sizer)
            sizer->Detach(pControl);
        pControl->Destroy();
    }

    if (subPages.size() != 0)
    {
        for (int i = 0, size = subPages.size(); i < size; ++i)
        {
            subPages.erase(subPages.begin());
        }
    }
}

Notebook::Page* Notebook::Page::addSubPage(Page* p)
{
    p->visible(false);
    subPages.push_back(p);
    return p;
}

void Notebook::Page::removeSubPage(Page* p)
{
    for (size_t i = 0, size = subPages.size(); i < size; i++)
        if (subPages[i]->name() == p->name())
        {
            subPages.erase(subPages.begin() + i);
            break;
        }
}

Notebook::Page* Notebook::Page::select(bool force)
{
    if (pSelected && !force) // already selected
        return this;
    Dispatcher::GUI::Post(this, &Notebook::Page::onSelectPage);
    return this;
}

void Notebook::Page::selectSubPage(Notebook::Page* subPage)
{
    // Lock the drawing to prevent annoying flickering
    WIP::Locker wip;
    GUILocker locker;
    wxWindowUpdateLocker updater(&pNotebook);

    pNotebook.pCacheTitle.clear();
    if (!pNotebook.pCaption.empty())
        pNotebook.pCacheTitle << pNotebook.pCaption << wxT(" :: ");
    pNotebook.pCacheTitle << caption();
    pNotebook.pCacheTitle << subPage->caption();

    // Show / Hide extra controls
    if (!pNotebook.pExtraControls.empty())
    {
        for (uint i = 0; i != pNotebook.pExtraControls.size(); ++i)
            pNotebook.pExtraControls[i]->Show(pDisplayExtraControls);
    }

    foreach (auto* page, pNotebook.pPages)
    {
        if (page->pControl)
        {
            if ((page->pSelected = (page == subPage)))
                pNotebook.pLastSelected = page;
            EnableRefreshForAllDatagrid(page->pControl, page->pSelected);
            pNotebook.pSizerForPages->Show(page->pControl, (page->pSelected), false);
        }
        else
            page->pSelected = false;
    }
    this->pSelected = true;

    pNotebook.forceRefresh();
}

void Notebook::Page::onSelectPage()
{
    if (subPages.size() > 0)
    {
        selectSubPage(subPages[0]);
        return;
    }
    // Lock the drawing to prevent annoying flickering
    WIP::Locker wip;
    GUILocker locker;
    wxWindowUpdateLocker updater(&pNotebook);

    pNotebook.pCacheTitle.clear();
    if (!pNotebook.pCaption.empty())
        pNotebook.pCacheTitle << pNotebook.pCaption << wxT(" :: ");
    pNotebook.pCacheTitle << caption();

    // Show / Hide extra controls
    if (!pNotebook.pExtraControls.empty())
    {
        for (uint i = 0; i != pNotebook.pExtraControls.size(); ++i)
            pNotebook.pExtraControls[i]->Show(pDisplayExtraControls);
    }

    pNotebook.pLastSelected = nullptr;

    foreach (auto* page, pNotebook.pPages)
    {
        if (page->pControl)
        {
            if ((page->pSelected = (page == this)))
                pNotebook.pLastSelected = page;
            EnableRefreshForAllDatagrid(page->pControl, page->pSelected);
            pNotebook.pSizerForPages->Show(page->pControl, (page->pSelected), false);
        }
        else
            page->pSelected = false;
    }

    pNotebook.forceRefresh();
}

void Notebook::enableRefreshForAllDatagrid(bool enabled)
{
    foreach (auto* page, pPages)
        EnableRefreshForAllDatagrid(page->pControl, enabled && page->selected());
}

void Notebook::Tabs::onMouseUp(wxMouseEvent& evt)
{
    if (!GUIIsLock())
    {
        // Lock the drawing to prevent annoying flickering
        WIP::Locker wip;
        GUILocker locker;

        const wxPoint p = evt.GetPosition();

        foreach (auto* page, pNotebook.pPages)
        {
            if (page->visible())
            {
                if (not page->selected() && page->pBoundingBox.Contains(p))
                {
                    bool accept = true;
                    // Can we change the page ?
                    pNotebook.onPageAccept(*page, accept);
                    if (accept)
                    {
                        // Selection of the page
                        page->select();
                        // Post event - The page has been changed
                        pNotebook.onPageChanged(*page);
                    }
                    break;
                }

                if (page->selected() && page->pBoundingBox.Contains(p) && page->subPages.size() > 0)
                {
                    for (int i = 0, size = page->subPages.size(); i < size; i++)
                    {
                        if (page->subPages[i]->pBoundingBox.Contains(p))
                        {
                            // Selection of the page
                            page->selectSubPage(page->subPages[i]);

                            break;
                        }
                    }
                    break;
                }
            }
        }
    }
    evt.Skip();
}

bool Notebook::select(const wxString& name, bool triggerEvents)
{
    // Lock the drawing to prevent annoying flickering
    WIP::Locker wip;
    GUILocker locker;

    foreach (auto* pageptr, pPages)
    {
        Page& page = *pageptr;
        if (page.name() == name)
        {
            if (!page.selected())
            {
                if (triggerEvents)
                {
                    bool accept = true;
                    // Can we change the page ?
                    onPageAccept(page, accept);
                    if (accept)
                    {
                        // Selection of the page
                        page.select();
                        // Post event - The page has been changed
                        onPageChanged(page);
                    }
                    else
                        return false;
                }
                else
                    page.select();
            }
            return true;
        }
    }
#ifndef NDEBUG
    String sname;
    wxStringToString(name, sname);
    logs.warning() << "notebook: impossible to select '" << sname << "'";
#endif
    return false;
}

Notebook::Page* Notebook::find(const wxString& name)
{
    foreach (auto* page, pPages)
    {
        if (page->name() == name)
            return page;
    }

#ifndef NDEBUG
    String sname;
    wxStringToString(name, sname);
    logs.warning() << "notebook: impossible to find '" << sname << "'";
#endif
    return nullptr;
}

void Notebook::set_page_visibility(const wxString& name, bool visibility)
{
    Page* page = find(name);
    if (page == nullptr)
        return;
    page->visible(visibility);
}

void Notebook::caption(const wxString& s)
{
    pCaption = s;
    if (pCaption.size() > 33)
    {
        pCaption.resize(30);
        pCaption += wxT("...");
    }
    pCacheTitle.clear();

    pCacheTitle.clear();
    if (!pCaption.empty())
        pCacheTitle << pCaption << wxT(" :: ");
    if (pLastSelected)
        pCacheTitle << pLastSelected->caption();

    Refresh();
}

void Notebook::Page::caption(const wxString& s)
{
    pCaption = s;
    if (pCaption.size() > 33)
    {
        pCaption.resize(30);
        pCaption += wxT("...");
    }

    pNotebook.Refresh();
}

} // namespace Component
} // namespace Antares
