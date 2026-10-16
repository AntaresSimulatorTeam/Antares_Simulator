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

void printWithColor(std::ostream& out, const std::string& message, Color color);

void setLocalTime(std::tm* tmBuffer, const std::time_t* now);

std::string eol();

} // namespace Antares::Logs

#endif
