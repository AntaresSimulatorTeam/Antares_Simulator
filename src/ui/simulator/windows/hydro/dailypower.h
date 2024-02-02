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
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/
#ifndef __ANTARES_APPLICATION_HYDROOW_HYDRO_DAILYPOWER_H__
#define __ANTARES_APPLICATION_HYDROOW_HYDRO_DAILYPOWER_H__

#include "../../toolbox/components/datagrid/component.h"
#include "../../toolbox/input/area.h"
#include <ui/common/component/panel.h>
#include "../../toolbox/components/button.h"

namespace Antares
{
namespace Window
{
namespace Hydro
{
class Dailypower : public Component::Panel, public Yuni::IEventObserver<Dailypower>
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

} // namespace Hydro
} // namespace Window
} // namespace Antares

#endif // __ANTARES_APPLICATION_HYDROOW_HYDRO_DAILYPOWER_H__
