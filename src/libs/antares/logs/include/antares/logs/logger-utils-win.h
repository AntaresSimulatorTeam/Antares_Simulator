// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#ifdef _WIN32

#include <windows.h>
#include <ctime>
#include <iostream>
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

void setConsoleColor(std::ostream& out, const Color& color)
{
    HANDLE handle = GetStdHandle((&out == &std::cerr) ? STD_ERROR_HANDLE : STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(handle, winColor(color));
}

class ColorEnabler
{
public:
    explicit ColorEnabler(std::ostream& out):
        out_(out)
    {
    }

    std::string tagColor(const LevelInfo& level);
    std::string msgColor(const LevelInfo& level);
    std::string removeColor();

private:
    std::ostream& out_;
};

std::string ColorEnabler::tagColor(const LevelInfo& level)
{
    setConsoleColor(out_, level.tagColor);
    return {};
}

std::string ColorEnabler::msgColor(const LevelInfo& level)
{
    setConsoleColor(out_, level.messageColor);
    return {};
}

std::string ColorEnabler::removeColor()
{
    setConsoleColor(out_, Color::none);
    return {};
}

void setLocalTime(std::tm* tmBuffer, const std::time_t* now)
{
    localtime_s(tmBuffer, now);
}

std::string eol()
{
    return "\r\n";
}

}

#endif
