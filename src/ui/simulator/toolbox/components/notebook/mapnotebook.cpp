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
#include "mapnotebook.h"
#include "../../resources.h"
#include <wx/dcbuffer.h>
#include <wx/sizer.h>
#include <wx/font.h>
#include "../../components/map/settings.h"
#include <antares/logs.h>
#include "../../../application/wait.h"
#include <ui/common/lock.h>
#include <wx/wupdlock.h>
#include "../refresh.h"
#include "../../../application/study.h"

#define NODE_DRAW_COLOR_VARIATION_LIGHT 10
#define NODE_DRAW_COLOR_VARIATION_DARK 5
#define LAYER_MAX_NUMBER 10

using namespace Yuni;

namespace Antares
{
namespace Component
{
BEGIN_EVENT_TABLE(MapNotebook::MapTabs, Notebook::Tabs)
EVT_PAINT(MapNotebook::MapTabs::onDraw)
END_EVENT_TABLE()

MapNotebook::MapNotebook(wxWindow* parent, Notebook::Orientation orientation) :
 Notebook::Notebook(parent, orientation)
{
    alignment(alignLeft);
    if (GetSizer())
        GetSizer()->Clear();
    if (pTabs != nullptr)
        delete pTabs;
    pTabs = new MapTabs(this, *this);
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

    OnMapLayerChanged.connect(this, &MapNotebook::onMapLayerChanged);
}

void MapNotebook::onMapLayerChanged(const wxString* text)
{
    if (find(*text, false) != nullptr)
        select(*text);
}

bool MapNotebook::remove(Page* page)
{
    page->~Page();
    return true;
}

MapNotebook::MapTabs::MapTabs(wxWindow* parent, Notebook& notebook) :
 Tabs(parent, notebook), undrawnLeftTabs(0), remainingRightTabs(0), pTabNameCtrl(nullptr)
{
    Connect(GetId(), wxEVT_MOTION, wxMouseEventHandler(MapTabs::onMouseMove), NULL, this);
    Connect(GetId(), wxEVT_LEAVE_WINDOW, wxMouseEventHandler(MapTabs::onMouseLeave), NULL, this);
    // add Page button
    addPageButton = new tabButton(
      "images/16x16/white_add_18.png", this, tabButton::btnNone, "images/16x16/grey_add_18.png");
    // right navigation button
    rightTabScroll = new tabButton("images/16x16/white_arrow_right_18.png",
                                   this,
                                   tabButton::btnRight,
                                   "images/16x16/grey_arrow_right_18.png");
    // left navigation button
    leftTabScroll = new tabButton("images/16x16/white_arrow_left_18.png",
                                  this,
                                  tabButton::btnLeft,
                                  "images/16x16/grey_arrow_left_18.png");
}

void MapNotebook::MapTabs::onDraw(wxPaintEvent&)
{
    sizingOffset = 0;
    // The font re-used for each drawing
    enum
    {
        fontSize = 8,
    };
    static const wxFont font(fontSize, wxSWISS, wxNORMAL, wxNORMAL, false, wxT("Tahoma"));

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
            // bounding box for the add page button

            drawOrientationTop(dc);

            addPageButton->drawButton(
              dc,
              pRect.GetWidth() - addPageButton->buttonImage->GetWidth(),
              (pRect.height >> 2) - (addPageButton->buttonImage->GetHeight() >> 2));
        }
    }
}

void MapNotebook::MapTabs::drawOrientationTop(wxDC& dc)
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
    dc.SetPen(wxPen(a2, 1, wxSOLID));
    dc.DrawLine(0, pRect.height - 1, pRect.width, pRect.height - 1);

    // compute the size of all tabs and draw the navigation buttons if necessary
    doSizing(dc);

    if (pNotebook.pDisplayTitle && pNotebook.pTabsVisible)
    {
        if (pNotebook.pAlwaysDisplayTab || pNotebook.pPages.size() > 2
            || (pNotebook.pPages.size() == 2 && Data::Study::Current::Get()->showAllLayer))
        {
            switch (pNotebook.pAlignment)
            {
            case alignLeft:
            {
                int pos = 15;
                pos += sizingOffset;
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

                for (int i = 0, size = pNotebook.pPages.size(); i < size; ++i)
                {
                    if (i >= undrawnLeftTabs)
                        drawItemTop(dc, pNotebook.pPages[i], pos, Notebook::alignLeft);
                    else
                    {
                        pNotebook.pPages[i]->pBoundingBox.x = 0;
                        pNotebook.pPages[i]->pBoundingBox.width = 0;
                    }
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
                pos += sizingOffset;
                for (int i = 0, size = pNotebook.pPages.size(); i < size; ++i)
                {
                    if (i >= undrawnLeftTabs)
                        drawItemTop(dc, pNotebook.pPages[i], pos, Notebook::alignLeft);
                }
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

void MapNotebook::MapTabs::doSizing(wxDC& dc)
{
    remainingRightTabs = 0;
    int totalSize = 18 + 21 + 15 + sizingOffset; // selected page button, add page button, left
                                                 // margin, and left scroll button (if any)
    int realTotalSize = totalSize;
    for (int i = 0, size = pNotebook.pPages.size(); i < size; i++)
    {
        if (i >= undrawnLeftTabs)
        {
            totalSize += pNotebook.pPages[i]->visible()
                           ? dc.GetTextExtent(pNotebook.pPages[i]->caption()).GetWidth() + 8 * 2
                           : 0;
        }
        realTotalSize += pNotebook.pPages[i]->visible()
                           ? dc.GetTextExtent(pNotebook.pPages[i]->caption()).GetWidth() + 8 * 2
                           : 0;
    }

    rightTabScroll->isVisible = false;
    if (totalSize >= pRect.GetWidth()) // the tabs at the right won't fit in, we have to draw the
                                       // right scrolling button
    {
        // draw right button
        rightTabScroll->drawButton(
          dc,
          pRect.GetWidth() - 21 - 18,
          (pRect.height >> 2) - (rightTabScroll->buttonImage->GetHeight() >> 2));
    }
    else if (realTotalSize <= pRect.GetWidth()) // the added size of all layers fit in the window
    {
        undrawnLeftTabs = 0;
    }

    leftTabScroll->isVisible = false;
    if (undrawnLeftTabs
        > 0) // some tabs on the left weren't drawn, we need to draw left scrolling button
    {
        sizingOffset = 8;
        // draw left button
        leftTabScroll->drawButton(
          dc, 1, (pRect.height >> 2) - (leftTabScroll->buttonImage->GetHeight() >> 2));
    }
}

void MapNotebook::MapTabs::drawItemTop(wxDC& dc, Page* page, int& pos, Notebook::Alignment align)
{
    static const wxColour colBlack(0, 0, 0);
    static const wxColour colGray(100, 100, 100);

    if (!page || !page->visible())
    {
        page->pBoundingBox.x = 0;
        page->pBoundingBox.width = 0;
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
        wxRect rect(pos - ((align == alignRight) ? (textExtent.GetWidth() + 8 * 2) : 0),
                    pRect.y + 3,
                    textExtent.GetWidth() + 8 * 2,
                    pRect.height - 6);
        if (pos + rect.GetWidth() >= pRect.GetWidth() - 21 - 15 || remainingRightTabs > 0)
        {
            remainingRightTabs++;
            return;
        }

        dc.SetBrush(wxBrush(colGray, wxTRANSPARENT));
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
            rect.SetWidth(rect.GetWidth() + 18); // adding 18 pixels for tab closing element

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
                dc.SetPen(wxPen(wxColour(85, 85, 85), 1, wxSOLID));
                break;
            default:
                dc.SetPen(wxPen(wxColour(0, 0, 0), 1, wxSOLID));
            }
            dc.SetBrush(*wxTRANSPARENT_BRUSH);
            dc.DrawRectangle(rect.x, rect.y, rect.width, rect.height);

            dc.SetTextForeground(colBlack);
            dc.DrawText(
              page->caption(), rect.x + 8, pRect.y + pRect.height / 2 - textExtent.GetHeight() / 2);

            // closing layer image
            wxBitmap* image = Resources::BitmapLoadFromFile("images/16x16/close.png");
            dc.DrawBitmap(*image, wxPoint(rect.x + rect.GetWidth() - 17, (pRect.height >> 2) - 2));

            static_cast<MapNotebook&>(pNotebook).closePageBoundingBox.SetX(rect.x + rect.width
                                                                           - 17);
            static_cast<MapNotebook&>(pNotebook).closePageBoundingBox.SetY((pRect.height >> 2) - 2);
            static_cast<MapNotebook&>(pNotebook).closePageBoundingBox.SetWidth(18);
            static_cast<MapNotebook&>(pNotebook).closePageBoundingBox.SetHeight(18);
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

void MapNotebook::MapTabs::onMouseUp(wxMouseEvent& evt)
{
    if (!GUIIsLock())
    {
        // Lock the drawing to prevent annoying flickering
        WIP::Locker wip;
        GUILocker locker;

        const wxPoint p = evt.GetPosition();

        for (size_t i = 0, size = pNotebook.pPages.size(); i < size; i++)
        {
            auto* page = pNotebook.pPages[i];

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
                    OnMapLayerChanged(&page->name());

                    if (pTabNameCtrl != nullptr)
                    {
                        pTabNameCtrl->Hide();
                    }
                }
                break;
            }
            else if (page->selected() && page->pBoundingBox.Contains(p))
            {
                if (static_cast<MapNotebook&>(pNotebook).closePageBoundingBox.Contains(p))
                    static_cast<MapNotebook&>(pNotebook).onPageDelete(*page);
                else
                {
                    if (pTabNameCtrl == nullptr)
                    {
                        pTabNameCtrl = new MapNotebook::TabTextCtrl(this, wxID_ANY, page);
                        pTabNameCtrl->Connect(
                          wxEVT_COMMAND_TEXT_ENTER,
                          wxCommandEventHandler(MapNotebook::TabTextCtrl::OnTextEnter));
                    }
                    else
                    {
                        pTabNameCtrl->SetPage(page);
                    }
                    pTabNameCtrl->SetFocus();
                }
                break;
            }
        }

        if (addPageButton->pBoundingBox->Contains(p))
        {
            // Post event - The page has been changed
            static_cast<MapNotebook&>(pNotebook).onAddPage();
        }

        if (leftTabScroll->isVisible)
            if (leftTabScroll->pBoundingBox->Contains(p))
                leftTabScroll->onMouseUp(evt);

        if (rightTabScroll->isVisible)
            if (rightTabScroll->pBoundingBox->Contains(p))
                rightTabScroll->onMouseUp(evt);
    }
    evt.Skip();
}

void MapNotebook::MapTabs::onMouseMove(wxMouseEvent& evt)
{
    wxPoint pos(evt.GetPosition().x, evt.GetPosition().y);

    if (addPageButton->pBoundingBox)
        if (addPageButton->pBoundingBox->Contains(pos))
        {
            addPageButton->onMouseEnter();
        }
        else
        {
            addPageButton->onMouseLeave();
        }

    if (leftTabScroll->isVisible)
        if (leftTabScroll->pBoundingBox->Contains(pos))
        {
            leftTabScroll->onMouseEnter();
        }
        else
        {
            leftTabScroll->onMouseLeave();
        }

    if (rightTabScroll->isVisible)
        if (rightTabScroll->pBoundingBox->Contains(pos))
        {
            rightTabScroll->onMouseEnter();
        }
        else
        {
            rightTabScroll->onMouseLeave();
        }

    return;
}

void MapNotebook::MapTabs::onMouseLeave(wxMouseEvent& evt)
{
    rightTabScroll->onMouseLeave();

    leftTabScroll->onMouseLeave();

    addPageButton->onMouseLeave();
}

MapNotebook::MapTabs::tabButton::tabButton(std::string imagePath,
                                           MapTabs* parentFrame,
                                           BtnType t,
                                           char* hoverImagePath) :
 hoverImage(nullptr), parentTabFrame(parentFrame), pBoundingBox(nullptr), drawHover(false), type(t)
{
    buttonImage = Resources::BitmapLoadFromFile(imagePath.c_str());
    isVisible = false;
    if (hoverImagePath != nullptr)
        hoverImage = Resources::BitmapLoadFromFile(hoverImagePath);
}

MapNotebook::MapTabs::tabButton::~tabButton()
{
    delete pBoundingBox;
    delete buttonImage;
}

void MapNotebook::MapTabs::tabButton::drawButton(wxDC& dc, int x, int y)
{
    coords = wxPoint(x, y);
    pBoundingBox
      = new wxRect(x - 1, 0, buttonImage->GetWidth() + 2, parentTabFrame->pRect.GetHeight());

    // The DC
    if (buttonImage)
    {
        isVisible = true;
        if (drawHover)
        {
            dc.DrawRectangle(*pBoundingBox);
            if (hoverImage != nullptr)
            {
                dc.DrawBitmap(*hoverImage, coords);
                drawHover = false;
                return;
            }
            drawHover = false;
        }

        dc.DrawBitmap(*buttonImage, coords);
    }
}

void MapNotebook::MapTabs::tabButton::onMouseUp(wxMouseEvent& evt)
{
    onClick(evt, type);
}

void MapNotebook::MapTabs::tabButton::onMouseEnter()
{
    drawHover = true;
    parentTabFrame->pNotebook.Refresh();
}

void MapNotebook::MapTabs::tabButton::onMouseLeave()
{
    drawHover = false;
    parentTabFrame->pNotebook.Refresh();
}

void MapNotebook::MapTabs::tabButton::onClick(wxMouseEvent&, BtnType t)
{
    if (isVisible)
        switch (t)
        {
        case btnLeft:
            parentTabFrame->undrawnLeftTabs--;
            parentTabFrame->pNotebook.Refresh();
            break;
        case btnRight:
            parentTabFrame->undrawnLeftTabs++;
            parentTabFrame->pNotebook.Refresh();
            break;
        default:
            break;
        }
}

void MapNotebook::TabTextCtrl::OnTextEnter(wxCommandEvent& evt)
{
    for (int i = 0, size = pNotebook->pPages.size(); i < size;
         ++i) // Check that there is no layer already having this name
    {
        if (pNotebook->pPages[i]->name() == GetValue() && pNotebook->pPages[i] != pPage)
        {
            logs.warning() << "There already is a layer named: "
                           << std::string(GetValue().mb_str());
            SetValue(pPage->name());
            evt.Skip();
            return;
        }
    }
    if (pPage != pNotebook->pPages[0])
    {
        auto study = Data::Study::Current::Get();

        auto layerItEnd = study->layers.end();
        for (auto layerIt = study->layers.begin(); layerIt != layerItEnd; layerIt++)
        {
            if (layerIt->second == pPage->name())
                layerIt->second = GetValue();
        }

        pPage->name(GetValue());
        pPage->caption(GetValue());
        OnMapLayerRenamed(&pPage->name());
        Hide();
        Antares::MarkTheStudyAsModified();
    }
    else
    {
        logs.warning() << "Layer All can't be renamed ";
        Hide();
    }

    evt.Skip();
}

} // namespace Component
} // namespace Antares