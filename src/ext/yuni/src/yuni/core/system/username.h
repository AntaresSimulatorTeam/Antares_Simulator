/*
 * Copyright 2007-2025, RTE (https://www.rte-france.com)
 * See AUTHORS.txt
 * SPDX-License-Identifier: MPL-2.0
 * This file is part of Antares-Simulator,
 * Adequacy and Performance assessment for interconnected energy networks.
 *
 * Antares_Simulator is free software: you can redistribute it and/or modify
 * it under the terms of the Mozilla Public Licence 2.0 as published by
 * the Mozilla Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * Antares_Simulator is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * Mozilla Public Licence 2.0 for more details.
 *
 * You should have received a copy of the Mozilla Public Licence 2.0
 * along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
 */

/*
** This file is part of libyuni, a cross-platform C++ framework (http://libyuni.org).
**
** This Source Code Form is subject to the terms of the Mozilla Public License
** v.2.0. If a copy of the MPL was not distributed with this file, You can
** obtain one at http://mozilla.org/MPL/2.0/.
**
** github: https://github.com/libyuni/libyuni/
** gitlab: https://gitlab.com/libyuni/libyuni/ (mirror)
*/
#pragma once
#include "../../yuni.h"

namespace Yuni::System
{
/*!
** \brief Retrieves the calling user's name
**
** On Unixes, the value will be read from the environment variables
** `LOGNAME`. On Windows, the method GetUserName will be used.
**
** Windows (from MSDN):
** If the current thread is impersonating another client, the GetUserName
** function returns the user name of the client that the thread is impersonating.
** \see http://msdn.microsoft.com/en-us/library/ms724432%28v=vs.85%29.aspx
**
** \param out Variable of type 'string' where the value will be appened
** \param emptyBefore True to empty the parameter `out` before
** \return True if the operation succeeded (a valid username has been found), false otherwise
*/
template<class StringT>
YUNI_DECL bool Username(StringT& out, bool emptyBefore = true);

} // namespace Yuni::System

#include "username.hxx"
