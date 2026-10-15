// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#ifndef _WIN32

#include <ctime>
#include <iosfwd>
#include <string>

#include "antares/logs/logger-utils.h"

namespace Antares::Logs
{

//! ANSI escape sequence for the given color, the reset sequence for Color::none
std::string linuxColor(const Color& color);

class ColorEnabler
{
public:
    explicit ColorEnabler(std::ostream& out /* unused */);

    std::string tagColor(const LevelInfo& level);
    std::string msgColor(const LevelInfo& level);
    std::string removeColor();

private:
    std::string getLinuxColor(const Color& color);
    std::string close_with_color_;
};

void setLocalTime(std::tm* tmBuffer, const std::time_t* now);

std::string eol();

} // namespace Antares::Logs

#endif
