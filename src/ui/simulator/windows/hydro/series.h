/*
** Copyright 2007-2024, RTE (https://www.rte-france.com)
** See AUTHORS.txt
** SPDX-License-Identifier: MPL-2.0
** This file is part of Antares-Simulator,
** Adequacy and Performance assesment for interconnected energy networks.
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
#ifndef __ANTARES_APPLICATION_WINDOWS_HYDRO_PREPRO_H__
#define __ANTARES_APPLICATION_WINDOWS_HYDRO_PREPRO_H__

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
