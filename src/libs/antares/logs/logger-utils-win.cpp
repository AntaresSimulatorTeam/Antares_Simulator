// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifdef _WIN32

#include <iostream>
#include <windows.h>

#include "antares/logs/logger-utils.h"

namespace Antares::Logs
{
namespace
{
constexpr WORD winDefault = 7; // FOREGROUND_RED|GREEN|BLUE, no intensity
constexpr WORD winRed = FOREGROUND_RED | FOREGROUND_INTENSITY;
constexpr WORD winYellow = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY;
constexpr WORD winGreen = FOREGROUND_GREEN | FOREGROUND_INTENSITY;
constexpr WORD winWhite = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE
                          | FOREGROUND_INTENSITY;

WORD winColor(const Color& color)
{
    switch (color)
    {
    case Color::red:
        return winRed;
    case Color::yellow:
        return winYellow;
    case Color::green:
        return winGreen;
    case Color::white:
        return winWhite;
    default:
        return winDefault; // reset
    }
}

DWORD winOut(const std::ostream& out)
{
    return (&out == &std::cerr) ? STD_ERROR_HANDLE : STD_OUTPUT_HANDLE;
}
} // namespace

void printWithColor(std::ostream& out, const std::string& message, Color color)
{
    if (color != Color::none)
    {
        const DWORD handle = winOut(out);
        SetConsoleTextAttribute(GetStdHandle(handle), winColor(color));
        out << message;
        SetConsoleTextAttribute(GetStdHandle(handle), winColor(Color::none));
        return;
    }
    out << message;
}

void setLocalTime(std::tm* tmBuffer, const std::time_t* now)
{
    localtime_s(tmBuffer, now);
}

std::string eol()
{
    return "\r\n";
}

} // namespace Antares::Logs

#endif
