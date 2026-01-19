// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_TOOLBOX_INPUT_AREA_H__
#define __ANTARES_TOOLBOX_INPUT_AREA_H__

#include <yuni/core/event.h>
#include <wx/panel.h>
#include <antares/study/study.h>
#include "input.h"
#include <wx/arrstr.h>
#include "../components/htmllistbox/component.h"

namespace Antares::Toolbox::InputSelector
{
/*!
** \brief Visual Component for displaying all available areas (and groups)
*/
class Area: public AInput, public Yuni::IEventObserver<Area>
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

} // namespace Antares::Toolbox::InputSelector

#endif // __ANTARES_TOOLBOX_INPUT_AREA_H__
