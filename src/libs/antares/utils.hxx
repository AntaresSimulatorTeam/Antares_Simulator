/*
** Copyright 2007-2018 RTE
** Authors: Antares_Simulator Team
**
** This file is part of Antares_Simulator.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** There are special exceptions to the terms and conditions of the
** license as they are applied to this software. View the full text of
** the exceptions in file COPYING.txt in the directory of this software
** distribution
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Antares_Simulator. If not, see <http://www.gnu.org/licenses/>.
**
** SPDX-License-Identifier: licenceRef-GPL3_WITH_RTE-Exceptions
*/
#ifndef __ANTARES_LIBS_UTILS_HXX__
#define __ANTARES_LIBS_UTILS_HXX__

#include <ctype.h>
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

template<class StringT>
void TransformNameIntoNameBefore390(const AnyString& name, StringT& out)
{
    if (not name.empty())
    {
        bool duppl = true;
        out.reserve(name.size());

        for (uint i = 0; i != name.size(); ++i)
        {
            const char c = name[i];
            if ((c >= 'a' and c <= 'z') or (c >= 'A' and c <= 'Z') or (c >= '0' and c <= '9')
                or c == '_' or c == '-' or c == '.')
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
        // out.toLower();
    }
    else
    {
        // Generate an arbitrary ID
        out << "id_" << ((size_t)(&out));
    }
}

} // namespace Antares

#endif // __ANTARES_LIBS_UTILS_HXX__
