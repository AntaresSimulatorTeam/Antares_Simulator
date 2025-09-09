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
#include "default.h"

namespace Yuni::Math::Random
{
inline void Default::reset()
{
    // Initializing the random generator with a standard seed
    ::srand((uint)::time(NULL));
    // Compute a random number for nothing, to avoid the first generated number
    ::rand();
}

inline void Default::reset(uint seed)
{
    // Initializing the random generator with the given seed
    ::srand(seed);
    // Compute a random number for nothing, to avoid the first generated number
    ::rand();
}

inline Default::Value Default::next()
{
    return rand();
}

inline Default::Value Default::min()
{
    return 0;
}

inline Default::Value Default::max()
{
    return RAND_MAX;
}

template<class U>
inline Default& Default::operator>>(U& u)
{
    u = next();
    return *this;
}

} // namespace Yuni::Math::Random
