/*
** Copyright 2007-2024, RTE (https://www.rte-france.com)
** See AUTHORS.txt
** SPDX-License-Identifier: MPL-2.0
** This file is part of Antares-Simulator,
** Adequacy and Performance assessment for interconnected energy networks.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the Mozilla Public Licence 2.0 as published by
** the Mozilla Foundation, either version 2 of the License, or
** (at your option) any later version.
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/
#ifndef __ANTARES_LIBS_UTILS_HXX__
#define __ANTARES_LIBS_UTILS_HXX__

#include <cctype>

#include <yuni/core/string.h>

namespace Antares
{
template<class StringT>
void TransformNameIntoID(const AnyString& name, StringT& out)
{
    if (not name.empty())
    {
        bool duppl = true;
        out.reserve(name.size());

        for (uint i = 0; i != name.size(); ++i)
        {
            const char c = name[i];
            if ((c >= 'a' and c <= 'z') or (c >= 'A' and c <= 'Z') or (c >= '0' and c <= '9')
                or c == '_' or c == '-' or c == '(' or c == ')' or c == ',' or c == '&' or c == ' ')
            {
                duppl = false;
                out += c;
            }
            else
            {
                if (not duppl)
                {
                    out += ' ';
                    duppl = true;
                }
            }
        }
        out.trim();
        out.toLower();
    }
    else
    {
        // Generate an arbitrary ID
        out << "id_" << ((size_t)(&out));
    }
}

} // namespace Antares

#endif // __ANTARES_LIBS_UTILS_HXX__
