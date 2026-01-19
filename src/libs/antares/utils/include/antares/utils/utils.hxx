// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

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

template<>
void TransformNameIntoID(const AnyString& name, std::string& out);

} // namespace Antares

#endif // __ANTARES_LIBS_UTILS_HXX__
