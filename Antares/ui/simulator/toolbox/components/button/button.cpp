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

#include "button.h"
#include "../../resources.h"
#include <wx/dcbuffer.h>
#include <wx/settings.h>
#include <wx/menu.h>
#include <wx/sizer.h>
#include <antares/logs.h>

using namespace Yuni;

namespace Antares
{
namespace Component
{
BEGIN_EVENT_TABLE(Button, Panel)
EVT_PAINT(Button::onDraw)
EVT_ERASE_BACKGROUND(Button::onEraseBackground)
END_EVENT_TABLE()

namespace // anonymous
{
enum
{
    captionMargin = 3,
    iconMargin = 2,
    recommendedHeight = 22,
    marginBetweenCaptionAndIcon = 2,
    dropDownWidth = 14,
};

// The font re-used for each drawing
enum
{
    fontSize = 8,
};
static const wxFont font(wxFontInfo(fontSize).Family(wxFONTFAMILY_SWISS).FaceName("Tahoma"));
static const wxFont fontBold(
  wxFontInfo(fontSize).Family(wxFONTFAMILY_SWISS).Bold().FaceName("Tahoma"));

static wxMenu* gPopupMenu = nullptr;

} // anonymous namespace

Button::Button(wxWindow* parent, const wxString& caption) :
 Panel(parent),
 pCaption(caption),
 pUserData(nullptr),
 pIcon(nullptr),
 pClickBehavior(clkDefault),
 pRecommendedWidth(0),
 pMiddleWidth(0),
 pSelected(false),
 pPushed(false),
 pAutoToggle(false),
 pBold(false),
 pHover(true)
{
    SetBackgroundStyle(wxBG_STYLE_CUSTOM); // Required by both GTK and Windows
    if (parent)
        SetBackgroundColour(parent->GetBackgroundColour());
    precalculateCoordinates();
}

Button::Button(wxWindow* parent, const wxString& caption, const char* bitmap) :
 Panel(parent),
 pCaption(caption),
 pUserData(nullptr),
 pIcon(nullptr),
 pClickBehavior(clkDefault),
 pRecommendedWidth(0),
 pMiddleWidth(0),
 pSelected(false),
 pPushed(false),
 pAutoToggle(false),
 pBold(false),
 pHover(true)
{
    SetBackgroundStyle(wxBG_STYLE_CUSTOM); // Required by both GTK and Windows
    if (parent)
        SetBackgroundColour(parent->GetBackgroundColour());
    loadIconFromResource(bitmap);
    precalculateCoordinates();
}

Button::Button(wxWindow* parent,
               const wxString& caption,
               const char* bitmap,
               const Button::OnClick& onclick) :
 Panel(parent),
 pCaption(caption),
 pOnClick(onclick),
 pUserData(nullptr),
 pIcon(nullptr),
 pClickBehavior(clkDefault),
 pRecommendedWidth(0),
 pMiddleWidth(0),
 pSelected(false),
 pPushed(false),
 pAutoToggle(false),
 pBold(false),
 pHover(true)
{
    SetBackgroundStyle(wxBG_STYLE_CUSTOM); // Required by both GTK and Windows
    if (parent)
        SetBackgroundColour(parent->GetBackgroundColour());
    loadIconFromResource(bitmap);
    precalculateCoordinates();
}

Button::~Button()
{
    // Disconnecting
    pOnClick.clear();
    pOnPopup.clear();

    delete pIcon;
    delete gPopupMenu;
    gPopupMenu = nullptr;
}

void Button::loadIconFromResource(const char* filename)
{
    delete pIcon;
    if (filename && '\0' != *filename)
    {
        pIcon = Resources::BitmapLoadFromFile(filename);
        if (pIcon && not pIcon->IsOk())
        {
            delete pIcon;
            pIcon = nullptr;
        }
    }
    else
        pIcon = nullptr;
}

void Button::precalculateCoordinates()
{
    // Assert
    assert(this != NULL && "Button: this must not be null");
    assert(iconMargin <= 128 && "Button: The margin seems suspicious");

    if (pCaption.empty())
    {
        pCaptionExtent.Set(0, 0);
        pRecommendedWidth = 1;
        pMiddleWidth = 0;
    }
    else
    {
        wxClientDC dc(this);
        if (not pBold)
            dc.SetFont(font);
        else
            dc.SetFont(fontBold);

        pCaptionExtent = dc.GetTextExtent(pCaption);
        pRecommendedWidth = pCaptionExtent.GetWidth() + (int)captionMargin + 2;
        pMiddleWidth = pRecommendedWidth / 2;
    }

    if (pIcon)
    {
        // Assert
        assert(pIcon->IsOk() && "Button: Invalid Icon");
        assert(pIcon->GetWidth() <= 128 && "Button: The width of the icon seems suspicious");
        assert(pIcon->GetHeight() <= 128 && "Button: The height of the icon seems suspicious");

        int v = iconMargin + pIcon->GetWidth();
        pRecommendedWidth += v + iconMargin;
        pMiddleWidth += v;
        if (!pCaption.empty())
        {
            pRecommendedWidth += (int)marginBetweenCaptionAndIcon;
            pMiddleWidth += (int)marginBetweenCaptionAndIcon;
        }
    }
    else
    {
        pRecommendedWidth += (int)captionMargin * 2;
        pMiddleWidth += (int)captionMargin;
    }

    if (pClickBehavior != clkDefault)
        pRecommendedWidth += (int)dropDownWidth;

    SetSize(pRecommendedWidth, (int)recommendedHeight);
    SetMinSize(wxSize(pRecommendedWidth, (int)recommendedHeight));
    wxSizer* sizer = const_cast<wxSizer*>(GetContainingSizer());
    if (sizer)
    {
        pSelected = false;
        sizer->SetItemMinSize(this, pRecommendedWidth, (int)recommendedHeight);
        sizer->Layout();
    }
}

void Button::onDraw(wxPaintEvent&)
{
    enum
    {
#ifdef YUNI_OS_WINDOWS
        // We need to modify the y-coordinate of the text on Windows
        decalY = -1,
#else
        decalY = 0
#endif
    };
    // The DC
    wxAutoBufferedPaintDC dc(this);
    // Shifts the device origin so we don't have to worry
    // about the current scroll position ourselves
    PrepareDC(dc);
    // Cute font
    if (not pBold)
        dc.SetFont(font);
    else
        dc.SetFont(fontBold);

    // Drawing the background
    wxRect rect = GetRect();
    const wxColour originalColor = GetBackgroundColour();

    // Redraw the background
    dc.SetPen(wxPen(originalColor, 1, wxPENSTYLE_SOLID));
    dc.SetBrush(wxBrush(originalColor, wxBRUSHSTYLE_SOLID));
    dc.DrawRectangle(0, 0, rect.GetWidth(), rect.GetHeight());

    // reducing the canvas
    rect.y = rect.height / 2 - (int)recommendedHeight / 2;
    rect.height = (int)recommendedHeight;
    rect.x = rect.width / 2 - pRecommendedWidth / 2;
    rect.width = pRecommendedWidth - 1;

    // erase
    // gradient
    rect.y += 1;
    rect.height -= 2;

    // Code for drawing a gradient, just in case
    // dc.GradientFillLinear(rect, wxColour(251, 251, 251), wxColour(195, 195, 195), wxSOUTH);

    // selection
    if (pSelected || pPushed)
    {
        wxColour a;
        if (pColorOverridePushed.IsOk())
        {
            a.Set((unsigned char)Math::MinMax<int>(pColorOverridePushed.Red() + 10, 0, 255),
                  (unsigned char)Math::MinMax<int>(pColorOverridePushed.Green() + 10, 0, 255),
                  (unsigned char)Math::MinMax<int>(pColorOverridePushed.Blue() + 10, 0, 255));
            dc.SetPen(wxPen(pColorOverridePushed, 1, wxPENSTYLE_SOLID));
            dc.SetBrush(wxBrush(a, wxBRUSHSTYLE_SOLID));
        }
        else
        {
            const wxColour& selectionColor = wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHT);
            a.Set((unsigned char)Math::MinMax<int>(selectionColor.Red() + 30, 0, 255),
                  (unsigned char)Math::MinMax<int>(selectionColor.Green() + 30, 0, 255),
                  (unsigned char)Math::MinMax<int>(selectionColor.Blue() + 30, 0, 255));
            dc.SetPen(wxPen(selectionColor, 1, wxPENSTYLE_SOLID));
            dc.SetBrush(wxBrush(a, wxBRUSHSTYLE_SOLID));
        }

        dc.DrawRectangle(rect);
        dc.SetTextForeground(wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHTTEXT));
        dc.DrawText(pCaption,
                    rect.x + pMiddleWidth - pCaptionExtent.GetWidth() / 2,
                    rect.y + rect.height / 2 - pCaptionExtent.GetHeight() / 2 + (int)decalY);
    }
    else
    {
        dc.SetPen(wxPen(originalColor, 1, wxPENSTYLE_SOLID));
        dc.SetBrush(wxBrush(originalColor, wxBRUSHSTYLE_SOLID));
        dc.DrawRectangle(rect);

        if (enabled())
            dc.SetTextForeground(
              GetForegroundColour()); // wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT));
        else
        {
            wxColour a((unsigned char)Math::MinMax<int>(originalColor.Red() - 80, 0, 255),
                       (unsigned char)Math::MinMax<int>(originalColor.Green() - 80, 0, 255),
                       (unsigned char)Math::MinMax<int>(originalColor.Blue() - 80, 0, 255));
            dc.SetTextForeground(a);
        }

        dc.DrawText(pCaption,
                    rect.x + pMiddleWidth - pCaptionExtent.GetWidth() / 2,
                    rect.y + rect.height / 2 - pCaptionExtent.GetHeight() / 2 + (int)decalY);
    }

    // icon
    if (pIcon)
    {
        // assert
        assert(pIcon->IsOk() && "Button: Invalid Icon");

        int x;
        if (!pCaption.empty())
        {
            x = rect.x + pMiddleWidth - pCaptionExtent.GetWidth() / 2 - pIcon->GetWidth()
                - iconMargin;
            x -= (int)marginBetweenCaptionAndIcon;
        }
        else
        {
            x = rect.x + rect.width / 2 - pIcon->GetWidth() / 2;
            if (pClickBehavior != clkDefault)
                x -= (int)dropDownWidth / 2;
        }

        dc.DrawBitmap(*pIcon, x, rect.y + rect.height / 2 - pIcon->GetHeight() / 2);
    }

    if (pClickBehavior != clkDefault)
    {
        int x = rect.x + rect.width - (int)dropDownWidth;
        if (pSelected)
        {
            if (pClickBehavior == clkDropdown)
            {
                dc.SetPen(
                  wxPen(wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHT), 1, wxPENSTYLE_SOLID));
                dc.DrawLine(x, rect.y + 2, x, rect.y + rect.height - 2);
            }
            dc.SetPen(
              wxPen(wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHTTEXT), 1, wxPENSTYLE_SOLID));
            dc.SetBrush(
              wxBrush(wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHTTEXT), wxBRUSHSTYLE_SOLID));
        }
        else
        {
            dc.SetPen(
              wxPen(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT), 1, wxPENSTYLE_SOLID));
            dc.SetBrush(
              wxBrush(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT), wxBRUSHSTYLE_SOLID));
        }

        wxPoint pPts[3];
        pPts[0].x = x + (int)dropDownWidth / 2;
        pPts[0].y = rect.y + rect.height / 2 + 1;
        pPts[1].x = pPts[0].x - 2;
        pPts[1].y = pPts[0].y - 2;
        pPts[2].x = pPts[0].x + 2;
        pPts[2].y = pPts[0].y - 2;
        dc.DrawPolygon(3, pPts);
    }
}

void Button::onMouseClick()
{
    // do nothing
}

void Button::onMouseEnter()
{
    auto newValue = enabled() && pHover; // can select only if enabled
    if (newValue != pSelected)
    {
        pSelected = newValue;
        Refresh();
    }
}

void Button::onMouseLeave()
{
    if (pSelected)
    {
        pSelected = false;
        Refresh();
    }
}

void Button::internalClick()
{
    // Update the focus to avoid segv from other components
    SetFocus();
    // Triggering event
    pOnClick(pUserData);
}

void Button::onMouseUp(wxMouseEvent& evt)
{
    // assert
    assert(wxIsMainThread() == true && "Must be ran from the main thread");

    if (pAutoToggle)
        pPushed = !pPushed;

    switch (pClickBehavior)
    {
    case clkDefault:
    {
        Yuni::Bind<void()> callback;
        callback.bind(this, &Button::internalClick);
        Dispatcher::GUI::Post(callback, 50);
        break;
    }
    default:
    {
        wxRect rect = GetClientRect();
        rect.y = rect.height / 2 - (int)recommendedHeight / 2;
        rect.height = (int)recommendedHeight;
        rect.x = rect.width / 2 - pRecommendedWidth / 2;
        rect.width = pRecommendedWidth - 1;

        if (pClickBehavior == clkDropdown)
        {
            if (evt.GetX() < rect.x + rect.width - (int)dropDownWidth)
            {
                Yuni::Bind<void()> callback;
                callback.bind(this, &Button::internalClick);
                Dispatcher::GUI::Post(callback, 50);
                break;
            }
        }
        // Popup
        wxMenu* popupMenu = new wxMenu();

        // Asking to the user to populate the menu
        pOnPopup(*this, *popupMenu, pUserData);

        // To avoid interfering with some special treatments by the user
        // we should not call PopupMenu if no item is present
        // (ex: the user may decide to popup a custom form)
        if (popupMenu->GetMenuItemCount() > 0)
            PopupMenu(popupMenu, 0, rect.height + rect.y - 2);
        else
        {
            delete popupMenu;
            popupMenu = nullptr;
        }
        if (gPopupMenu)
        {
            delete gPopupMenu;
            gPopupMenu = popupMenu;
        }
        break;
    }
    }
}

void Button::caption(const wxString& rhs)
{
    pCaption = rhs;
    precalculateCoordinates();
    if (GetParent())
        GetParent()->Layout();
    Refresh();
}

void Button::caption(const wxChar* rhs)
{
    if (rhs)
        pCaption = rhs;
    else
        pCaption.clear();
    precalculateCoordinates();
    if (GetParent())
        GetParent()->Layout();
    Refresh();
}

} // namespace Component
} // namespace Antares
