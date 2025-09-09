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
#include "../string.h"

namespace Yuni::System::Environment
{
/*!
** \brief Get a value from the current environment and convert it as a bool
**
** \param name Name of the variable
** \return true if present and equal to "1", "on" or "true"
*/
YUNI_DECL bool ReadAsBool(const AnyString& name);

/*!
** \brief Get a value from the current environment and convert it as an integer
**
** \param name Name of the variable
** \param defvalue The default value to take if the value of the variable is empty
*/
YUNI_DECL int64_t ReadAsInt64(const AnyString& name, int64_t defvalue = 0);

/*!
** \brief Get a value from the current environment
**
** \param name Name of the variable
** \return The value of the variable, empty is an error has occured
*/
YUNI_DECL String Read(const AnyString& name);

/*!
** \brief Get a value from the current environment (Yuni::String version)
**
** \param name Name of the variable
** \param[out] out Variable of type 'string'/'container' where the value will be appened
** \param emptyBefore True to empty the parameter `out` before
** \return True if the operation succeeded, false otherwise
*/
YUNI_DECL bool Read(const AnyString& name, String& out, bool emptyBefore = true);

/*!
** \brief Get a value from the current environment (Yuni::Clob version)
**
** \param name Name of the variable
** \param[out] out Variable of type 'string'/'container' where the value will be appened
** \param emptyBefore True to empty the parameter `out` before
** \return True if the operation succeeded, false otherwise
*/
YUNI_DECL bool Read(const AnyString& name, Clob& out, bool emptyBefore = true);

} // namespace Yuni::System::Environment
