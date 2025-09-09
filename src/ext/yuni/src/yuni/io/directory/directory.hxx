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
#include "directory.h"
#include "../../core/traits/cstring.h"
#include "../../core/traits/length.h"
#include "../../core/static/remove.h"
#include "../constants.h"
#ifdef YUNI_HAS_STDLIB_H
#include <stdlib.h>
#endif

namespace Yuni::Private::IO::Directory
{
inline bool DummyCopyUpdateEvent(Yuni::IO::Directory::CopyState,
                                 const String&,
                                 const String&,
                                 uint64_t,
                                 uint64_t)
{
    return true;
}

} // namespace Yuni::Private::IO::Directory

namespace Yuni::IO::Directory
{
inline bool Exists(const AnyString& path)
{
    return (IO::typeFolder == IO::TypeOf(path));
}

inline bool Copy(const AnyString& source,
                 const AnyString& destination,
                 bool recursive,
                 bool overwrite)
{
    CopyOnUpdateBind e;
    e.bind(&Private::IO::Directory::DummyCopyUpdateEvent);
    return Copy(source, destination, recursive, overwrite, e);
}

inline bool Copy(const AnyString& source,
                 const AnyString& destination,
                 const CopyOnUpdateBind& onUpdate)
{
    return Copy(source, destination, true, true, onUpdate);
}

} // namespace Yuni::IO::Directory
