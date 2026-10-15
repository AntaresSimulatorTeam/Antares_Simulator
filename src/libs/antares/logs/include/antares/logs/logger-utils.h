// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <ctime>
#include <iosfwd>
#include <string>

namespace Antares::Logs
{
enum class Color
{
    none,
    red,
    yellow,
    green,
    white
};

struct LevelInfo
{
    int level;
    const char* tag;
    bool toStderr;
    bool notifyCallback;
    Color tagColor;
    Color messageColor;
};

void printWithColor(std::ostream& out, const std::string& message, Color color);

void setLocalTime(std::tm* tmBuffer, const std::time_t* now);

std::string eol();

} // namespace Antares::Logs
