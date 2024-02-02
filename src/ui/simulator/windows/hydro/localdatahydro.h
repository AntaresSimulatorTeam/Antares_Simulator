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
#ifndef __ANTARES_APPLICATION_WINDOWS_HYDRO_LOCALDATAHYDRO_H__
#define __ANTARES_APPLICATION_WINDOWS_HYDRO_LOCALDATAHYDRO_H__

#include "../../toolbox/input/area.h"
#include "../../toolbox/components/notebook/notebook.h"
#include <ui/common/component/panel.h>

namespace Antares
{
namespace Window
{
namespace Hydro
{
class Localdatahydro : public Component::Panel, public Yuni::IEventObserver<Localdatahydro>
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

} // namespace Hydro
} // namespace Window
} // namespace Antares

#endif // __ANTARES_APPLICATION_WINDOWS_HYDRO_LOCALDATAHYDRO_H__
