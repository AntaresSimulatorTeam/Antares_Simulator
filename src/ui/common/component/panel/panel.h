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
#ifndef __ANTARES_COMMON_COMPONENTS_PANEL_PANEL_H__
#define __ANTARES_COMMON_COMPONENTS_PANEL_PANEL_H__

#include <yuni/yuni.h>
#include "../../wx-wrapper.h"
#include <wx/panel.h>

namespace Antares
{
namespace Component
{
/*!
** \brief Panel implementation
*/
class Panel : public wxPanel
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

} // namespace Component
} // namespace Antares

#endif // __ANTARES_COMMON_COMPONENTS_PANEL_PANEL_H__
