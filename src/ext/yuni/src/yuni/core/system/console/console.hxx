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
#include "../windows.hdr.h"
#include "console.h"

namespace Yuni::System::Console
{
#ifdef YUNI_OS_WINDOWS

template<class U>
inline void ResetTextColor(U&)
{
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
                            FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
}

template<>
struct TextColor<black> final
{
    template<class U>
    static void Set(U&)
    {
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0);
    }
};

template<>
struct TextColor<none> final
{
    template<class U>
    static void Set(U&)
    {
    }
};

template<>
struct TextColor<white> final
{
    template<class U>
    static void Set(U&)
    {
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
                                FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN
                                  | FOREGROUND_BLUE);
    }
};

template<>
struct TextColor<red> final
{
    template<class U>
    static void Set(U&)
    {
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
                                FOREGROUND_INTENSITY | FOREGROUND_RED);
    }
};

template<>
struct TextColor<green> final
{
    template<class U>
    static void Set(U&)
    {
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
                                FOREGROUND_INTENSITY | FOREGROUND_GREEN);
    }
};

template<>
struct TextColor<yellow> final
{
    template<class U>
    static void Set(U&)
    {
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
                                FOREGROUND_INTENSITY | FOREGROUND_GREEN | FOREGROUND_RED);
    }
};

template<>
struct TextColor<blue> final
{
    template<class U>
    static void Set(U&)
    {
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
                                FOREGROUND_INTENSITY | FOREGROUND_BLUE);
    }
};

template<>
struct TextColor<purple> final
{
    template<class U>
    static void Set(U&)
    {
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
                                FOREGROUND_INTENSITY | FOREGROUND_BLUE | FOREGROUND_RED);
    }
};

template<>
struct TextColor<lightblue> final
{
    template<class U>
    static void Set(U&)
    {
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
                                FOREGROUND_INTENSITY | FOREGROUND_BLUE | FOREGROUND_GREEN);
    }
};

template<>
struct TextColor<gray> final
{
    template<class U>
    static void Set(U&)
    {
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
                                FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
    }
};

template<>
struct TextColor<bold> final
{
    template<class U>
    static void Set(U&)
    {
    }
};

#else

template<>
struct TextColor<none>
{
    template<class U>
    static void Set(U&)
    {
    }
};

template<>
struct TextColor<black> final
{
    template<class U>
    static void Set(U& out)
    {
        out << "[1;30m";
    }
};

template<>
struct TextColor<red> final
{
    template<class U>
    static void Set(U& out)
    {
        out << "[0;31m";
    }
};

template<>
struct TextColor<green> final
{
    template<class U>
    static void Set(U& out)
    {
        out << "[0;32m";
    }
};

template<>
struct TextColor<blue> final
{
    template<class U>
    static void Set(U& out)
    {
        out << "[0;34m";
    }
};

template<>
struct TextColor<yellow> final
{
    template<class U>
    static void Set(U& out)
    {
        out << "[0;33m";
    }
};

template<>
struct TextColor<purple> final
{
    template<class U>
    static void Set(U& out)
    {
        out << "[0;35m";
    }
};

template<>
struct TextColor<lightblue> final
{
    template<class U>
    static void Set(U& out)
    {
        out << "[0;36m";
    }
};

template<>
struct TextColor<gray> final
{
    template<class U>
    static void Set(U& out)
    {
        out << "[0;37m";
    }
};

template<>
struct TextColor<white> final
{
    template<class U>
    static void Set(U& out)
    {
        out << "[1;37m[1m";
    }
};

template<>
struct TextColor<bold> final
{
    template<class U>
    static void Set(U& out)
    {
        out << "[1m";
    }
};

template<class U>
inline void ResetTextColor(U& out)
{
    out << "[0m";
}

#endif

template<class U>
inline void SetTextColor(U& out, const Color color)
{
    switch (color)
    {
    case black:
        TextColor<black>::Set(out);
        break;
    case red:
        TextColor<red>::Set(out);
        break;
    case green:
        TextColor<green>::Set(out);
        break;
    case yellow:
        TextColor<yellow>::Set(out);
        break;
    case blue:
        TextColor<blue>::Set(out);
        break;
    case purple:
        TextColor<purple>::Set(out);
        break;
    case lightblue:
        TextColor<lightblue>::Set(out);
        break;
    case gray:
        TextColor<gray>::Set(out);
        break;
    case white:
        TextColor<white>::Set(out);
        break;
    case bold:
        TextColor<bold>::Set(out);
        break;
    case none:
        break;
    }
}

} // namespace Yuni::System::Console
