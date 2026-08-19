// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#ifdef _WIN32

#include <ctime>
#include <iostream>
#include <windows.h>

#include "antares/logs/logger-utils.h"

namespace Antares::Logs
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

class ColorEnabler
{
public:
    explicit ColorEnabler(std::ostream& out):
        out_(winOut(out))
    {
    }

    std::string tagColor(const LevelInfo& level);
    std::string msgColor(const LevelInfo& level);
    std::string removeColor();

private:
    DWORD out_;
};

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
