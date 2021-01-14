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
#ifndef __ANTARES_APPLICATION_WINDOWS_HYDRO_PREPRO_H__
#define __ANTARES_APPLICATION_WINDOWS_HYDRO_PREPRO_H__

#include <antares/wx-wrapper.h>
#include "../../toolbox/input/area.h"
#include "../../toolbox/components/notebook/notebook.h"
#include <ui/common/component/panel.h>

namespace Antares
{
namespace Window
{
namespace Hydro
{
class Series : public Component::Panel, public Yuni::IEventObserver<Series>
{
public:
    //! name Constructor & Destructor
    //@{
    /*!
    ** \brief Default constructor
    */
    Series(wxWindow* parent, Toolbox::InputSelector::Area* notifier = NULL);
    //! Destructor
    virtual ~Series();
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

}; // class Series

} // namespace Hydro
} // namespace Window
} // namespace Antares

#endif // __ANTARES_APPLICATION_WINDOWS_HYDRO_PREPRO_H__
