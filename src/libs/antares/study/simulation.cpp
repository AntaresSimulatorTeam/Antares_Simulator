/*
** Copyright 2007-2024, RTE (https://www.rte-france.com)
** See AUTHORS.txt
** SPDX-License-Identifier: MPL-2.0
** This file is part of Antares-Simulator,
** Adequacy and Performance assessment for interconnected energy networks.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the Mozilla Public Licence 2.0 as published by
** the Mozilla Foundation, either version 2 of the License, or
** (at your option) any later version.
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/

#include "antares/study/simulation.h"

#include <stdio.h>
#include <string.h>

#include <yuni/yuni.h>
#include <yuni/io/file.h>

#include <antares/logs/logs.h>
#include "antares/antares/constants.h"
#include "antares/study/study.h"

using namespace Yuni;

#define SEP IO::Separator

namespace Antares
{
namespace Data
{
SimulationComments::SimulationComments(Study& study):
    pStudy(study)
{
}

bool SimulationComments::saveToFolder(const AnyString& folder) const
{
    String b;
    b.reserve(folder.size() + 20);
    b = folder;

    // Ensure that the folder has been created
    if (!IO::Directory::Create(b))
    {
        logs.error() << "I/O: impossible to create the directory " << b;
        return false;
    }

    // Save the comments
    b = folder;
    b << SEP << "comments.txt";
    if (IO::File::SetContent(b, comments))
    {
        return true;
    }
    logs.error() << "I/O: impossible to write " << b;
    return false;
}

void SimulationComments::saveUsingWriter(Solver::IResultWriter& writer,
                                         const AnyString& folder) const
{
    String b = folder;
    b << SEP << "comments.txt";
    std::string comments_copy = comments.c_str();
    writer.addEntryFromBuffer(b.c_str(), comments_copy);
}

bool SimulationComments::loadFromFolder(const StudyLoadOptions& options)
{
    if (!options.loadOnlyNeeded)
    {
        pStudy.buffer.clear() << pStudy.folderSettings << SEP << "comments.txt";
        if (IO::errNone != IO::File::LoadFromFile(comments, pStudy.buffer))
        {
            logs.warning() << pStudy.buffer << ": Impossible to read the file";
        }
    }
    return true;
}

uint64_t SimulationComments::memoryUsage() const
{
    return name.capacity() + comments.capacity();
}

} // namespace Data
} // namespace Antares
