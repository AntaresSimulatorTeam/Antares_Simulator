// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_LIBS_SYS_APPDATA_H__
#define __ANTARES_LIBS_SYS_APPDATA_H__

#include <yuni/yuni.h>
#include <yuni/core/string.h>

namespace OperatingSystem
{
/*!
** \brief Try to find out the local app data folder
**
** \param allusers True to retrieve the folder associated to the `all users`
*/
bool FindLocalAppData(YString& out, bool allusers = false);

/*!
** \brief Try to find out the local app data folder
**
** \param allusers True to retrieve the folder associated to the `all users`
*/
bool FindAntaresLocalAppData(YString& out, bool allusers = false);

} // namespace OperatingSystem

#endif // __ANTARES_LIBS_SYS_APPDATA_H__
