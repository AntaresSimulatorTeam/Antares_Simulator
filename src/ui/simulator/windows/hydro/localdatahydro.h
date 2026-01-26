// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_APPLICATION_WINDOWS_HYDRO_LOCALDATAHYDRO_H__
#define __ANTARES_APPLICATION_WINDOWS_HYDRO_LOCALDATAHYDRO_H__

#include "../../toolbox/input/area.h"
#include "../../toolbox/components/notebook/notebook.h"
#include <ui/common/component/panel.h>

namespace Antares::Window::Hydro
{
class Localdatahydro: public Component::Panel, public Yuni::IEventObserver<Localdatahydro>
{
public:
    //! name Constructor & Destructor
    //@{
    /*!
    ** \brief Default constructor
    */
    Localdatahydro(wxWindow* parent, Toolbox::InputSelector::Area* notifier = NULL);
    //! Destructor
    virtual ~Localdatahydro();
    //@}

private:
    void onStudyLoaded();
    void onStudyClosed();

private:
    //! Event: An area has been selected
    void onAreaChanged(Data::Area* area);
    //! Area notifier
    Toolbox::InputSelector::Area* pNotifier;
    //! The last selected area
    Data::Area* pLastArea;
    //! Page
    Component::Notebook::Page* pPageFatal;

}; // class Localdatahydro

} // namespace Antares::Window::Hydro

#endif // __ANTARES_APPLICATION_WINDOWS_HYDRO_LOCALDATAHYDRO_H__
