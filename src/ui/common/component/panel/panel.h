// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_COMMON_COMPONENTS_PANEL_PANEL_H__
#define __ANTARES_COMMON_COMPONENTS_PANEL_PANEL_H__

#include <yuni/yuni.h>
#include "../../wx-wrapper.h"
#include <wx/panel.h>

namespace Antares::Component
{
/*!
** \brief Panel implementation
*/
class Panel: public wxPanel
{
public:
    /*!
    ** \brief Event triggered by any other component (not derived from Panel)
    */
    static void OnMouseMoveFromExternalComponent();

public:
    //! \name Constructor & Destructor
    //@{
    /*!
    ** \brief Constructor
    */
    Panel(wxWindow* parent);
    //! Destructor
    virtual ~Panel();

    //@}

    //! Click down
    virtual void onMouseDown(wxMouseEvent&)
    {
    }

    //! Click up
    virtual void onMouseUp(wxMouseEvent&)
    {
    }

    //! The panel has been clicked (delayed)
    virtual void onMouseClick()
    {
    }

    //! The mouse has entered
    virtual void onMouseEnter()
    {
    }

    //! The mouse has leaved
    virtual void onMouseLeave()
    {
    }

    //! The mouse has moved
    virtual void onMouseMoved(int /*x*/, int /*y*/)
    {
    }

protected:
    virtual bool triggerMouseClickEvent() const
    {
        return true;
    }

private:
    //!
    void onInternalMotion(wxMouseEvent&);
    void onInternalMouseUp(wxMouseEvent&);
    void onInternalMouseDown(wxMouseEvent&);

}; // class Panel

} // namespace Antares::Component

#endif // __ANTARES_COMMON_COMPONENTS_PANEL_PANEL_H__
