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

namespace Yuni::Extension::CString
{
template<class CStringT, class T, class Alloc>
class Append<CStringT, Yuni::Nullable<T, Alloc>> final
{
    static void Perform(CStringT& s, const Yuni::Nullable<T, Alloc>& rhs)
    {
        if (!rhs.null())
        {
            s << rhs.value();
        }
    }
};

template<class T, class Alloc>
class Into<Yuni::Nullable<T, Alloc>> final
{
public:
    typedef Yuni::Nullable<T, Alloc> TargetType;

    enum
    {
        valid = 1
    };

    template<class StringT>
    static bool Perform(const StringT& s, TargetType& out)
    {
        T tmp;
        if (s.to(tmp))
        {
            out = tmp;
        }
        else
        {
            out = nullptr;
        }
        return true;
    }

    template<class StringT>
    static TargetType Perform(const StringT& s)
    {
        return s.template to<T>();
    }
};

} // namespace Yuni::Extension::CString

template<typename T, class Alloc>
inline std::ostream& operator<<(std::ostream& out, const Yuni::Nullable<T, Alloc>& rhs)
{
    rhs.print(out, "(null)");
    return out;
}
