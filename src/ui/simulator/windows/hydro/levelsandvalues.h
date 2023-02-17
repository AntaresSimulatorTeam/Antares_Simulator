/*
** Copyright 2007-2023 RTE
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
#ifndef __ANTARES_APPLICATION_HYDROOW_HYDRO_LEVELSANDVALUES_H__
#define __ANTARES_APPLICATION_HYDROOW_HYDRO_LEVELSANDVALUES_H__

#include <antares/wx-wrapper.h>
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
class LevelsAndValues : public Component::Panel, public Yuni::IEventObserver<LevelsAndValues>
{
public:
    //! \name Constructor & Destructor
    //@{
    /*!
    ** \brief Constructor
    */
    LevelsAndValues(wxWindow* parent, Toolbox::InputSelector::Area* notifier);
    //! Destructor
    virtual ~LevelsAndValues();
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

}; // class Prepro

} // namespace Hydro
} // namespace Window
} // namespace Antares

#endif // __ANTARES_APPLICATION_HYDROOW_HYDRO_LEVELSANDVALUES_H__
