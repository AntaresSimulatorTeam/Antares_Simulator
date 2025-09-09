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
#include "../yuni.h"
#include "../core/string.h"

namespace Yuni::DateTime
{
/*!
** \brief Unix Timestamp
**
** A timestamp is a time in seconds since 0 hours, 0 minutes, 0 seconds,
** January 1, 1970, Coordinated Universal Time.
*/
typedef /*time_t*/ int64_t Timestamp;

/*!
** \brief Get the current timestamp (in seconds since epoch)
*/
Timestamp Now();

/*!
** \brief Get the current timestamp (in milliseconds since epoch)
*/
Timestamp NowMilliSeconds();

/*!
** \brief Convert an Unix timestamp to a string
**
** \code
** String s;
** DateTime::TimestampToString(s, "%D");
** \endcode
**
** \param[out] out The string where the result will be written
** \param format Format (man strftime). The string must be zero-terminated
** \param timestamp An Unix Timestamp (0 or negative value will be replaced by the current
*timestamp)
** \param emptyBefore True to empty the string before anything else
** \return True if the operation succeded, false otherwise
*/
template<class StringT>
bool TimestampToString(StringT& out,
                       const AnyString& format,
                       Timestamp timestamp = 0,
                       bool emptyBefore = true);

} // namespace Yuni::DateTime

#include "timestamp.hxx"
