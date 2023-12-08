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
** SPDX-License-Identifier: MPL 2.0
*/

#include <string>
#include <yuni/yuni.h>
#include "study.h"
#include <antares/logs/logs.h>
#include <yuni/io/file.h>

using namespace Yuni;

namespace Antares
{
namespace Data
{
void Study::importLogsToOutputFolder(Solver::IResultWriter& resultWriter) const
{
    if (!logs.logfile())
        return;

    std::string logPath("simulation.log");
    String from;
    IO::Normalize(from, logs.logfile());

    if (System::windows)
    {
        // On Windows, the access file is exclusive by default.
        // So we have to close the file before copying it.
        logs.closeLogfile();
    }

    resultWriter.addEntryFromFile(logPath, from.c_str());

    if (System::windows)
    {
        // Reopen the log file
        logs.logfile(from);
    }
}

} // namespace Data
} // namespace Antares
