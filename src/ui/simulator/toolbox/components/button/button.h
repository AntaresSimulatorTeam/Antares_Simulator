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
#ifndef __ANTARES_TOOLBOX_COMPONENTS_BUTTON_BUTTON_H__
#define __ANTARES_TOOLBOX_COMPONENTS_BUTTON_BUTTON_H__

#include <yuni/yuni.h>
#include <antares/wx-wrapper.h>
#include <wx/bitmap.h>
#include <ui/common/component/panel.h>
#include <vector>

namespace Antares
{
namespace Component
{
class Button final : public Panel
{
public:
    //! Event: User click
    using OnClick = Yuni::Bind<void(void*)>;
    //! Event: Popup menu
    using OnPopupMenu = Yuni::Bind<void(Button&, wxMenu&, void*)>;
    //! Vector
    using Vector = std::vector<Button*>;

    enum ClickBehavior
    {
        clkDefault,
        clkDropdown,
        clkMenu,
    };

public:
    //! \name Constructor & Destructor
    //@{
    /*!
    ** \brief Constructor
    */
    Button(wxWindow* parent, const wxString& caption);
    /*!
    ** \brief Constructor
    */
    Button(wxWindow* parent, const wxString& caption, const char* bitmap);
    /*!
    ** \brief Constructor, with a callback
    */
    Button(wxWindow* parent, const wxString& caption, const char* bitmap, const OnClick& onclick);

    /*!
    ** \brief Constructor, with a callback
    */
    template<class T>
    Button(wxWindow* parent,
           const wxString& caption,
           const char* bitmap,
           T* object,
           void (T::*method)(void*));

    //! Destructor
    virtual ~Button();
    //@}

    //! \name Caption
    //@{
    //! Get the caption of the button
    const wxString& caption() const;
    //! Set the caption of the button
    void caption(const wxString& rhs);
    void caption(const wxChar* rhs);
    //@}

    //! \name Image
    //@{
    //! Set the image filename
    void image(const char* filename);
    //@}

    //! \name Enabled
    //@{
    //! Get if the button is enabled
    bool enabled() const;
    //! Set if the button is enabled
    void enabled(bool value);
    //@}

    //! \name User-data
    //@{
    //! Get the user-data
    void* userdata() const;
    //! Set the user-data
    void userdata(void* v);
    //@}

    //! \name Drop down
    //@{
    //! Get if the button has a drop down menu
    bool dropDown() const;
    //! Set if the button has a drop down menu
    void dropDown(bool rhs);
    //@}

    //! \name Menu
    //@{
    //! Get if a click on the button will produce a menu
    bool menu() const;
    //! Set if a click on the button should produce a menu
    void menu(bool rhs);
    //@}

    //! \name Toggle
    //@{
    //! Get if the button is pushed
    bool pushed() const;
    //! Set if the button is pushed
    void pushed(bool v);

    //! Get if the button is in auto-toggle mode
    bool autoToggle() const;
    //! Set if the button is in auto-toggle mode
    void autoToggle(bool v);

    //! Set the pushed color
    void pushedColor(uint r, uint g, uint b);
    void pushedColor(const wxColour& color);
    void pushedColor(const wxWindow* wnd);
    //@}

    //! \name Bold
    //@{
    //! Get if the font should be bold
    bool bold() const;
    //! Set the font weight
    void bold(bool value);
    //@}

    //! \name Hover
    //@{
    //! Determine whether a selection should appear when the mouse is over the component
    void hover(bool value);
    bool hover() const;
    //@}

    /*!
    ** \brief Set the handler for creating the popup menu
    */
    template<class T>
    void onPopupMenu(const T* object, void (T::*method)(Button&, wxMenu&, void*));
    /*!
    ** \brief Set the handler for creating the popup menu
    */
    void onPopupMenu(const OnPopupMenu& popup);

    /*!
    ** \brief Set the onClick event
    */
    template<class T>
    void onClick(const T* object, void (T::*method)(void*));

    //! Disconnect the onClick event
    void disconnectClickEvent();

    //! The panel has been clicked (delayed)
    virtual void onMouseClick() override;

    //! The mouse has entered
    virtual void onMouseEnter() override;
    //! The mouse has leaved
    virtual void onMouseLeave() override;

    //! Click up
    virtual void onMouseUp(wxMouseEvent& evt);

protected:
    //! Preload the Icon
    void loadIconFromResource(const char* filename);
    //! Pre-calculate all coordinates
    void precalculateCoordinates();
    //! internal click
    void internalClick();

    //! Event: draw the panel
    void onDraw(wxPaintEvent& evt);
    //! UI: Erase background, empty to avoid flickering
    void onEraseBackground(wxEraseEvent&)
    {
    }

    //! Tells to Component::Panel to not trigger its own mouse click event
    // (may lead to SegV otherwise)
    virtual bool triggerMouseClickEvent() const override
    {
        return false;
    }

private:
    //! Caption of the button
    wxString pCaption;
    //! Event: onClick
    OnClick pOnClick;
    //! Event: onPopupMenu
    OnPopupMenu pOnPopup;
    //! User-data
    void* pUserData;
    //! Icon
    wxBitmap* pIcon;
    //! Click behavior
    ClickBehavior pClickBehavior;

    //! The recommended size
    int pRecommendedWidth;
    int pMiddleWidth;
    bool pSelected;
    bool pPushed;
    bool pAutoToggle;
    bool pBold;
    bool pHover;

    //! Override the color for push button
    wxColour pColorOverridePushed;

    //! Caption extent
    wxSize pCaptionExtent;
    // Event table
    DECLARE_EVENT_TABLE()

}; // class Button

} // namespace Component
} // namespace Antares

#include "button.hxx"

#endif // __ANTARES_TOOLBOX_COMPONENTS_BUTTON_BUTTON_H__
