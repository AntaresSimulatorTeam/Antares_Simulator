// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/logs/logger-utils-linux.h"

#ifndef _WIN32

#include <ostream>

namespace Antares::Logs
{

namespace
{
std::string linuxColor(const Color& color)
{
    switch (color)
    {
    case Color::red:
        return "\x1b[0;31m";
    case Color::yellow:
        return "\x1b[0;33m";
    case Color::green:
        return "\x1b[0;32m";
    case Color::white:
        return "\x1b[1;37m";
    default:
        return "\x1b[0m"; // reset
    }
}
} // namespace

void printWithColor(std::ostream& out, const std::string& message, Color color)
{
    if (color != Color::none)
    {
        out << linuxColor(color) << message << linuxColor(Color::none);
        return;
    }
    out << message;
}

void setLocalTime(std::tm* tmBuffer, const std::time_t* now)
{
    localtime_r(now, tmBuffer);
}

std::string eol()
{
    return "\n";
}

} // namespace Antares::Logs

#endif
