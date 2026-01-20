// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_COMMON_COMPONENT_SPOTLIGHT_LISTBOX_PANEL_H__
#define __ANTARES_COMMON_COMPONENT_SPOTLIGHT_LISTBOX_PANEL_H__

#include <yuni/yuni.h>
#include "../../wx-wrapper.h"
#include <wx/scrolwin.h>
#include "spotlight.h"

namespace Antares::Component
{
class ListboxPanel: public wxScrolledWindow, public Yuni::IEventObserver<ListboxPanel>
{
public:
    enum
    {
        searchResultTextHeight = 25,
    };

public:
    //! \name Constructor & Destructor
    //@{
    /*!
    ** \brief Default constructor
    */
    ListboxPanel(Spotlight* parent, uint flags);
    //! Destructor
    virtual ~ListboxPanel();
    //@}

    void updateItems(const Spotlight::IItem::VectorPtr& vptr,
                     const Spotlight::SearchToken::VectorPtr& tokens);

    /*!
    ** \brief Reset the data provider
    **
    ** It is likely that the provider has been changed from the component.
    */
    void resetProvider(Spotlight::IProvider::Ptr provider);

protected:
    //! Event: draw the panel
    void onDraw(wxPaintEvent& evt);
    //! Event: mouse moved
    void onMouseMotion(wxMouseEvent& evt);
    //! Event: mouse leave
    void onMouseLeave(wxMouseEvent&);

    //! UI: Erase background, empty to avoid flickering
    void onEraseBackground(wxEraseEvent&)
    {
    }

    //! Recompute the bounds of the longuest group name
    void recomputeBoundsForLonguestGroupName(wxDC& dc);

    //! Event: Mouse Click
    void onMouseLeftUp(wxMouseEvent& evt);
    //! Event: Mouse Double Click
    void onMouseDblClick(wxMouseEvent& evt);
    //! Delayed event
    void doSelectItemFromMouseLeftUp();
    //! Delayed event
    void doSelectItemFromMouseDblClick();

private:
    //! The parent window
    Spotlight* pParent;
    //! Items
    Spotlight::IItem::VectorPtr pItems;
    //! Tokens
    Spotlight::SearchToken::VectorPtr pTokens;
    //! The longuest group name
    wxString pLonguestGroupName;
    wxSize pLonguestGroupNameSize;
    bool pRecomputeLonguestGroupNameSize;
    wxColour pDisabledColor;
    //!
    wxString pCacheResultText;
    wxSize pCacheResultTextSize;
    //
    uint pMouseOverItem;
    //! Provider
    Spotlight::IProvider::Ptr pProvider;
    //! Flags
    uint pFlags;

    //! Cache for last group name - used by onDraw
    YString pLastGroupName;

protected:
    // Event table
    DECLARE_EVENT_TABLE()

}; // class ListboxPanel

} // namespace Antares::Component

#endif // __ANTARES_COMMON_COMPONENT_SPOTLIGHT_LISTBOX_PANEL_H__
