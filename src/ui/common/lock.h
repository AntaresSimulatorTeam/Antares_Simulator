// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

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
