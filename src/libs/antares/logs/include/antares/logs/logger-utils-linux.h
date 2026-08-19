// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#ifndef _WIN32

#include <ctime>
#include "antares/logs/logger-utils.h"

namespace Antares::Logs
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

class ColorEnabler
{
public:
    explicit ColorEnabler([[maybe_unused]] std::ostream& out /* unused */)
    {
    }

    std::string tagColor(const LevelInfo& level);
    std::string msgColor(const LevelInfo& level);
    std::string removeColor();

private:
    std::string getLinuxColor(const Color& color);
    std::string close_with_color_;
};

std::string ColorEnabler::getLinuxColor(const Color& color)
{
    close_with_color_ = "";
    if (color != Color::none)
    {
        close_with_color_ = linuxColor(Color::none);
        return linuxColor(color);
    }
    return {};
}

std::string ColorEnabler::tagColor(const LevelInfo& level)
{
    return getLinuxColor(level.tagColor);
}

std::string ColorEnabler::msgColor(const LevelInfo& level)
{
    return getLinuxColor(level.messageColor);
}

std::string ColorEnabler::removeColor()
{
    return close_with_color_;
}

void setLocalTime(std::tm* tmBuffer, const std::time_t* now)
{
    localtime_r(tmBuffer, now);
}

std::string eol()
{
    return "\n";
}

}

#endif
