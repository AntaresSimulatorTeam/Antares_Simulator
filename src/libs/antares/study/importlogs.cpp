/*
** Copyright 2007-2018 RTE
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

#include <yuni/yuni.h>
#include "study.h"
#include "../logs.h"
#include <yuni/io/file.h>

using namespace Yuni;

namespace Antares
{
namespace Data
{
void Study::importLogsToOutputFolder() const
{
    if (!logs.logfile())
        return;

    String path;
    path.reserve(this->folderOutput.size() + 20);

    // Double check: Since this method might be call after some memory starvation,
    // we should make sure that we have a valid buffer
    if (!path.data())
    {
        logs.error() << "I/O Error: Impossible to create the folder '" << path << "'";
        return;
    }
    path << "simulation.log";
    String from;
    IO::Normalize(from, logs.logfile());

    if (System::windows)
    {
        // On Windows, the access file is exclusive by default.
        // So we have to close the file before copying it.
        logs.closeLogfile();
    }

    Yuni::Clob log_buffer;
    // Handle errors
    switch (IO::File::LoadFromFile(log_buffer, from))
    {
    case IO::errNone:
        break;
    case IO::errNotFound:
        logs.error() << from << ": file does not exist";
        path.clear().shrink();
        break;
    case IO::errReadFailed:
        logs.error() << "Read failed '" << from << "'";
        path.clear().shrink();
        break;
    default:
        logs.error() << "Impossible to read '" << from << "' to '" << path << "'";
        path.clear().shrink();
        break;
    }

    pResultWriter->addJob(path.c_str(), log_buffer);

    if (System::windows)
    {
        // Reopen the log file
        logs.logfile(from);
    }
}

} // namespace Data
} // namespace Antares
