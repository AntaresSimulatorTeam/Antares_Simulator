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

#include <antares/logs.h>
#include <string_view>
#include "log_sink.h"

namespace Antares::Solver
{
void LogSink::send(LogSeverity severity,
                   const char* /* full_filename */,
                   const char* /* base_filename */,
                   int /* line */,
                   const struct ::tm* /* tm_time */,
                   const char* message,
                   size_t message_len)
{
    /* NOTES
     1. Our logger handles filenames, timestamps, etc. We don't need those.
     2. Message *is not* '\0'-terminated, we must use it's length.
        Also, we use std::string_view to avoid copies
    */
    const std::string_view msg(message, message_len);
    switch (severity)
    {
    case google::GLOG_INFO:
        Antares::logs.info() << msg;
        break;
    case google::GLOG_WARNING:
        Antares::logs.warning() << msg;
        break;
    case google::GLOG_ERROR:
        Antares::logs.error() << msg;
        break;
    case google::GLOG_FATAL:
        Antares::logs.fatal() << msg;
        break;
    default:
        Antares::logs.warning() << "Unknown log level " << msg;
    }
}
} // namespace Antares::Solver
