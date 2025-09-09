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
#include "../../core/static/remove.h"
#include "../../core/traits/cstring.h"
#include "../../core/traits/length.h"
#include "file.h"
#include "stream.h"

namespace Yuni::IO::File
{
inline uint64_t Size(const AnyString& filename)
{
    uint64_t size;
    return (Size(filename, size)) ? size : 0;
}

inline bool Exists(const AnyString& filename)
{
    Yuni::IO::NodeType type = Yuni::IO::TypeOf(filename);
    return (Yuni::IO::typeFolder != type and Yuni::IO::typeUnknown != type);
}

template<class U>
bool SetContent(const AnyString& filename, const U& content)
{
    IO::File::Stream file(filename, OpenMode::write | OpenMode::truncate);
    if (file.opened())
    {
        file += content;
        return true;
    }
    return false;
}

template<class U>
bool AppendContent(const AnyString& filename, const U& content)
{
    IO::File::Stream file(filename, OpenMode::write | OpenMode::append);
    if (file.opened())
    {
        file += content;
        return true;
    }
    return false;
}

template<class U>
bool SaveToFile(const AnyString& filename, const U& content)
{
    IO::File::Stream file;
    if (file.openRW(filename))
    {
        file += content;
        return true;
    }
    return false;
}

template<class PredicateT>
bool ReadLineByLine(const AnyString& filename, const PredicateT& predicate)
{
    IO::File::Stream file;
    if (file.open(filename))
    {
        String line;
        while (file.readline<4096u, String>(line))
        {
            predicate(line);
        }

        return true;
    }
    return false;
}

} // namespace Yuni::IO::File
