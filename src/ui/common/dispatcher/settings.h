// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_COMMON_ACTION_SETTINGS_H__
#define __ANTARES_COMMON_ACTION_SETTINGS_H__

#include "../wx-wrapper.h"

namespace Antares::Dispatcher::Settings
{
/*!
** \brief The wx window to use for performing pending events
*/
extern wxWindow* WindowForPendingDispatchers;

/*!
** \brief The wxID to use for performing pending events
*/
extern int IDForWxDispatcher;

} // namespace Antares::Dispatcher::Settings

#endif // __ANTARES_COMMON_ACTION_SETTINGS_H__
