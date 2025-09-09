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
#include <cstdlib>
#include "timestamp.h"

namespace Yuni::Private::DateTime
{
// forward declaration
char* FormatTimestampToString(const AnyString& format, int64_t timestamp);

} // namespace Yuni::Private::DateTime

namespace Yuni::DateTime
{
template<class StringT>
inline bool TimestampToString(StringT& out,
                              const AnyString& format,
                              Timestamp timestamp,
                              bool emptyBefore)
{
    if (emptyBefore)
    {
        out.clear();
    }
    if (format.empty())
    {
        return true;
    }

    char* buffer = Yuni::Private::DateTime::FormatTimestampToString(format, timestamp);
    if (buffer)
    {
        out += (const char*)buffer;
        ::free(buffer);
        return true;
    }
    return false;
}

} // namespace Yuni::DateTime
