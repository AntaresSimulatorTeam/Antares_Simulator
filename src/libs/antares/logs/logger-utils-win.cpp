// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/logs/logger-utils-win.h"

#ifdef _WIN32

#include <iostream>

namespace Antares::Logs
{

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

std::string setConsoleColor(DWORD& out, const Color& color)
{
    SetConsoleTextAttribute(GetStdHandle(out), winColor(color));
    // We need to return a string to match the Linux implementation, but on Windows we
    // don't use ANSI escape codes.
    return {};
}

DWORD winOut(const std::ostream& out)
{
    return (&out == &std::cerr) ? STD_ERROR_HANDLE : STD_OUTPUT_HANDLE;
}

ColorEnabler::ColorEnabler(std::ostream& out):
    out_(winOut(out))
{
}

std::string ColorEnabler::tagColor(const LevelInfo& level)
{
    return setConsoleColor(out_, level.tagColor);
}

std::string ColorEnabler::msgColor(const LevelInfo& level)
{
    return setConsoleColor(out_, level.messageColor);
}

std::string ColorEnabler::removeColor()
{
    return setConsoleColor(out_, Color::none);
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
