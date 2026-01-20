// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_TOOLBOX_COMPONENTS_WIP_PANEL_WIP_PANEL_H__
#define __ANTARES_TOOLBOX_COMPONENTS_WIP_PANEL_WIP_PANEL_H__

#include <yuni/yuni.h>
#include <wx/bitmap.h>
#include <ui/common/component/panel.h>

namespace Antares::Component
{
class WIPPanel: public Panel
{
public:
    //! \name Constructor & Destructor
    //@{
    /*!
    ** \brief Constructor
    */
    WIPPanel(wxWindow* parent);

    //! Destructor
    virtual ~WIPPanel();
    //@}

protected:
    //! Event: draw the panel
    void onDraw(wxPaintEvent& evt);

    //! UI: Erase background, empty to avoid flickering
    void onEraseBackground(wxEraseEvent&)
    {
    }

private:
    //! Icon
    wxBitmap* pIcon;
    // Event table
    DECLARE_EVENT_TABLE()

}; // class WIPPanel

} // namespace Antares::Component

#endif // __ANTARES_TOOLBOX_COMPONENTS_WIP_PANEL_WIP_PANEL_H__
