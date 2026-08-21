// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#ifdef _WIN32

#include <ctime>
#include <iosfwd>
#include <string>
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

//! Console attribute for the given color, the default attribute for Color::none
WORD winColor(const Color& color);

std::string setConsoleColor(DWORD& out, const Color& color);

//! Standard handle id matching the given stream
DWORD winOut(const std::ostream& out);

class ColorEnabler
{
public:
    explicit ColorEnabler(std::ostream& out);

    std::string tagColor(const LevelInfo& level);
    std::string msgColor(const LevelInfo& level);
    std::string removeColor();

private:
    DWORD out_;
};

void setLocalTime(std::tm* tmBuffer, const std::time_t* now);

std::string eol();

} // namespace Antares::Logs

#endif
