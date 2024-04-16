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

#include "antares/utils/utils.h"

#include <sstream>

#include <antares/logs/logs.h>

using namespace Yuni;

namespace Antares
{
void
BeautifyName(YString& out, AnyString oldname)
{
    out.clear();
    if (oldname.empty())
    {
        return;
    }

    oldname.trim(" \r\n\t");
    if (oldname.empty())
    {
        return;
    }

    out.reserve(oldname.size());

    auto end = oldname.utf8end();
    for (auto i = oldname.utf8begin(); i != end; ++i)
    {
        auto& utf8char = *i;
        // simple char
        char c = (char)utf8char;

        if (c == ' ' or (c >= 'a' and c <= 'z') or (c >= 'A' and c <= 'Z') or
            (c >= '0' and c <= '9') or c == '_' or c == '-' or c == '(' or c == ')' or c == ',' or
            c == '&')
        {
            out += c;
        }
        else
        {
            out += ' ';
        }
    }

    out.trim(" \t\r\n");

    while (std::string(out.c_str()).find("  ") != std::string::npos)
    {
        out.replace("  ", " ");
    }

    out.trim(" \t\r\n");
}

template<>
void
TransformNameIntoID(const AnyString& name, std::string& out)
{
    Yuni::String yuniOut;
    TransformNameIntoID(name, yuniOut);
    out = yuniOut;
}

void
BeautifyName(std::string& out, const std::string& oldname)
{
    YString yuniOut;
    BeautifyName(yuniOut, oldname);
    out = yuniOut.c_str();
}

std::vector<std::pair<std::string, std::string>>
splitStringIntoPairs(const std::string& s, char delimiter1, char delimiter2)
{
    std::vector<std::pair<std::string, std::string>> pairs;
    std::stringstream ss(s);
    std::string token;

    while (std::getline(ss, token, delimiter1))
    {
        size_t pos = token.find(delimiter2);
        if (pos != std::string::npos)
        {
            std::string begin = token.substr(0, pos);
            std::string end = token.substr(pos + 1);
            pairs.push_back({begin, end});
        }
        else
        {
            logs.warning() << "Error while parsing: " << token;
        }
    }

    return pairs;
}

namespace Utils
{

bool
isZero(double d)
{
    constexpr double threshold = 1.e-6;
    return std::abs(d) < threshold;
}

double
round(double d, unsigned precision)
{
    unsigned factor = std::pow(10, precision);
    return std::round(d * factor) / factor;
}

} // namespace Utils
} // namespace Antares
