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
#ifndef __ANTARES_UI_COMMON_LOCK_H__
#define __ANTARES_UI_COMMON_LOCK_H__

#include <yuni/yuni.h>

namespace Antares
{
/*!
** \brief The component should not perform their updates
*/
void GUIBeginUpdate();

/*!
** \brief The component may perform their graphical updates
*/
void GUIEndUpdate();

/*!
** \brief Get if the component can perform their updates
*/
bool GUIIsLock();

//! Get the internal ref count value
uint GUILockRefCount();

/*!
** \brief Lock / Unlock the GUI (RAII)
*/
class GUILocker final
{
public:
    GUILocker()
    {
        GUIBeginUpdate();
    }

    ~GUILocker()
    {
        GUIEndUpdate();
    }
};

/*!
** \brief Mark the GUI as 'about to quit', to avoid useless and sometimes costly refresh
*/
void GUIIsAboutToQuit();

/*!
** \brief The application is no longer quitting (ex: the user canceled the operation)
*/
void GUIIsNoLongerQuitting();

/*!
** \brief Get if the GUI is about to quit
*/
bool IsGUIAboutToQuit();

} // namespace Antares

#endif // __ANTARES_UI_COMMON_LOCK_H__
