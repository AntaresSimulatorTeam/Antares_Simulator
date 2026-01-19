// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_APPLICATION_HYDROOW_HYDRO_DAILYPOWER_H__
#define __ANTARES_APPLICATION_HYDROOW_HYDRO_DAILYPOWER_H__

#include "../../toolbox/components/datagrid/component.h"
#include "../../toolbox/input/area.h"
#include <ui/common/component/panel.h>
#include "../../toolbox/components/button.h"

namespace Antares::Window::Hydro
{
class Dailypower: public Component::Panel, public Yuni::IEventObserver<Dailypower>
{
public:
    //! \name Constructor & Destructor
    //@{
    /*!
    ** \brief Constructor
    */
    Dailypower(wxWindow* parent, Toolbox::InputSelector::Area* notifier);
    //! Destructor
    virtual ~Dailypower();
    //@}

private:
    void createComponents();
    void onStudyClosed();
    void onAreaChanged(Data::Area* area);

private:
    //! The input area selector
    Toolbox::InputSelector::Area* pInputAreaSelector;
    Data::Area* pArea;
    bool pComponentsAreReady;
    Component::Panel* pSupport;

}; // class Dailypower

} // namespace Antares::Window::Hydro

#endif // __ANTARES_APPLICATION_HYDROOW_HYDRO_DAILYPOWER_H__
