// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

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

namespace Antares::Data
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

} // namespace Antares::Data
