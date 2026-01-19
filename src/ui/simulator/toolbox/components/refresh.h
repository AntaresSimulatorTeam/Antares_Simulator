// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_TOOLBOX_COMPONENTS_REFRESH_H__
#define __ANTARES_TOOLBOX_COMPONENTS_REFRESH_H__

class wxWindow;

namespace Antares
{
/*!
** \brief Refresh all components in an efficient way
*/
void RefreshAllControls(wxWindow* comObj);

/*!
** \brief Enable / Disable the refresh for all datagrids contained by a window
*/
void EnableRefreshForAllDatagrid(wxWindow* comObj, bool enabled);

} // namespace Antares

#endif // __ANTARES_TOOLBOX_COMPONENTS_REFRESH_H__
