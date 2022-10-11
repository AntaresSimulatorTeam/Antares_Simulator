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

using namespace Yuni;

#define SEP IO::Separator

namespace Antares
{
namespace Data
{
FILE* Study::createFileIntoOutputWithExtension(const YString& prefix,
                                               const YString& extension,
                                               uint numSpace,
                                               const int currentOptimNumber) const
{
    static std::map<YString, int> count;

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

    String outputFile;
    outputFile << prefix << "-"; // problem ou criterion
    outputFile << (runtime->currentYear[numSpace] + 1) << "-"
               << (runtime->weekInTheYear[numSpace] + 1);

    if (currentOptimNumber)
        outputFile << "--optim-nb-" << currentOptimNumber;

    buffer.clear() << this->folderOutput << SEP << outputFile;

    // test if file already exists
    FILE* fd_test = FileOpen(buffer.c_str(), "rb");
    if (fd_test)
    {
        count[prefix]++;
        outputFile << "-" << count[prefix] << "." << extension;
        fclose(fd_test);
    }
    else
    {
        count[prefix] = 0;
        outputFile << "." << extension;
    }

    buffer.clear() << this->folderOutput << SEP << outputFile;

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
