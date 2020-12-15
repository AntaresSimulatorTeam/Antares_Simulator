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

#include "study.h"
#include "../files/files.h"
#include "../logs.h"
#include <yuni/datetime/timestamp.h>

using namespace Yuni;

#define SEP IO::Separator

namespace Antares
{
namespace Data
{
FILE* Study::createFileIntoOutputWithExtension(const YString& prefix,
                                               const YString& extension,
                                               uint numSpace) const
{
    static int count_problem = 0;
    static int count_criterion = 0;

    // Empty log entry
    logs.info();

    // Preparing the filename where to write the MPS file
    String buffer;
    buffer.reserve(folderOutput.size() + 15);
    buffer << this->folderOutput;

    if (not IO::Directory::Create(buffer))
    {
        logs.error() << "I/O Error: Impossible to create the folder `" << buffer << "'";
        logs.info() << "Aborting now.";
        return nullptr;
    }

    // Date/time
    String outputFile;
    outputFile << prefix << "-"; // problem ou criterion
    outputFile << (runtime->currentYear[numSpace] + 1) << "-"
               << (runtime->weekInTheYear[numSpace] + 1) << "-";
    Yuni::DateTime::TimestampToString(outputFile, "%Y%m%d-%H%M%S", 0, false);
    String tempOutputFile;
    tempOutputFile << outputFile << "." << extension;

    buffer.clear() << this->folderOutput << SEP << tempOutputFile;

    // tester si le fichier existe deja
    FILE* fd_test = FileOpen(buffer.c_str(), "rb");
    // logs.debug() << " !fd_test = " << (!fd_test) << " : FOR " << buffer.c_str();
    if (fd_test)
    {
        if (prefix == "problem")
            outputFile << "-" << (++count_problem) << "." << extension;
        else
            outputFile << "-" << (++count_criterion) << "." << extension;

        buffer.clear() << this->folderOutput << SEP << outputFile;
        fclose(fd_test);
    }
    else
    {
        count_problem = count_criterion = 0;

        outputFile << "." << extension;
        buffer.clear() << this->folderOutput << SEP << outputFile;
    }

    logs.info() << "Solver output File: `" << buffer << "'";

    FILE* fd = FileOpen(buffer.c_str(), "wb");
    if (!fd)
    {
        logs.error() << "I/O Error: Impossible to write `" << buffer << "'";
        logs.info() << "Aborting now.";
        importLogsToOutputFolder();
        return nullptr;
    }
    logs.info();
    return fd;
}

} // namespace Data
} // namespace Antares
