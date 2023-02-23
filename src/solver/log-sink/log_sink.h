/*
** Copyright 2007-2023 RTE
** Authors: Antares_Simulator Team
**
** This file is part of Antares_Simulator.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** There are special exceptions to the terms and conditions of the
** license as they are applied to this software. View the full text of
** the exceptions in file COPYING.txt in the directory of this software
** distribution
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Antares_Simulator. If not, see <http://www.gnu.org/licenses/>.
**
** SPDX-License-Identifier: licenceRef-GPL3_WITH_RTE-Exceptions
*/

#pragma once

// Log sink class to be used with OR-Tools
// By default, OR-Tools writes its log to stderr
// This can be changed by providing a "log sink" as defined
// below. This is roughly equivalent to a callback with a state.

// Note that as of version 9.x, OR-Tools redefines the GLOG
// library. Including glog/logging.h will result in compilation
// errors (symbol redifinition)
#include <ortools/base/logging.h>
namespace Antares::Solver
{
class LogSink : public google::LogSink
{
    void send(LogSeverity severity,
              const char* /* full_filename */,
              const char* /* base_filename */,
              int /* line */,
              const struct ::tm* /* tm_time */,
              const char* message,
              size_t message_len) override;
};
} // namespace Antares::Solver
