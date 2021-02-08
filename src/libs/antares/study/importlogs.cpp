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

    String buffer;
    buffer.reserve(this->folderOutput.size() + 20);

    // Double check: Since this method might be call after some memory starvation,
    // we should make sure that we have a valid buffer
    if (!buffer.data())
    {
        logs.error() << "I/O Error: Impossible to create the folder '" << buffer << "'";
        return;
    }
    buffer << this->folderOutput;
    if (not IO::Directory::Create(buffer))
        return;

    buffer << IO::Separator << "simulation.log";
    logs.info() << " Writing log file: " << buffer;
    String from;
    IO::Normalize(from, logs.logfile());

    if (System::windows)
    {
        // On Windows, the access file is exclusive by default.
        // So we have to close the file before copying it.
        logs.closeLogfile();
    }

    switch (IO::File::Copy(from, buffer))
    {
    case IO::errNone:
        break;
    case IO::errOverwriteNotAllowed:
        logs.error() << from << ": File already exists";
        buffer.clear().shrink();
        break;
    case IO::errMemoryLimit:
        logs.error() << "Hard limit reached: Impossible to copy '" << from << "'";
        buffer.clear().shrink();
        break;
    default:
        logs.error() << "Impossible to copy '" << from << "' to '" << buffer << "'";
        buffer.clear().shrink();
        break;
    }

    if (System::windows)
    {
        // Reopen the log file
        logs.logfile(from);
    }
}

} // namespace Data
} // namespace Antares
