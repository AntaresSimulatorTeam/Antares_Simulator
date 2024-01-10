/*
** Copyright 2007-2024, RTE (https://www.rte-france.com)
** See AUTHORS.txt
** SPDX-License-Identifier: MPL-2.0
** This file is part of Antares-Simulator,
** Adequacy and Performance assessment for interconnected energy networks.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the Mozilla Public Licence 2.0 as published by
** the Mozilla Foundation, either version 2 of the License, or
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
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/
#ifndef __ANTARES_TOOLBOX_INPUT_AREA_H__
#define __ANTARES_TOOLBOX_INPUT_AREA_H__

#include <yuni/core/event.h>
#include <wx/panel.h>
#include <antares/study/study.h>
#include "input.h"
#include <wx/arrstr.h>
#include "../components/htmllistbox/component.h"

namespace Antares
{
namespace Toolbox
{
namespace InputSelector
{
/*!
** \brief Visual Component for displaying all available areas (and groups)
*/
class Area : public AInput, public Yuni::IEventObserver<Area>
{
public:
    //! \name Constructor & Destructor
    //@{
    /*!
    ** \brief Default Constructor
    ** \param parent The parent window
    */
    Area(wxWindow* parent);
    //! Destructor
    virtual ~Area();
    //@}

    virtual wxPoint recommendedSize() const
    {
        return wxPoint(230, 70);
    }

    /*!
    ** \brief Get the last area selected by the component
    */
    static Data::Area* lastArea();

    void reloadLastArea();

    virtual void update();

public:
    /*!
    ** \brief Public event, triggered when the selected area has changed
    */
    static Yuni::Event<void(Data::Area*)> onAreaChanged;

protected:
    virtual void internalBuildSubControls();

    /*!
    ** \brief Clear the control and broadcast a change in the current selected area
    */
    void clear();

    //! Callback: draw event
    void onDraw(wxPaintEvent& evt);

    //! The study has been updated
    void onStudyEndUpdate();

private:
    void internalSelectionChanged();
    void onApplicationOnQuit();
    void onStudyClosed();

private:
    //! Panel
    static wxPanel* pSharedSupport;
    //! The listbox, which will contains all items
    // static Component::HTMLListbox::Component* pAreaListbox;
    // Event table
    DECLARE_EVENT_TABLE()

}; // class Area

} // namespace InputSelector
} // namespace Toolbox
} // namespace Antares

#endif // __ANTARES_TOOLBOX_INPUT_AREA_H__
