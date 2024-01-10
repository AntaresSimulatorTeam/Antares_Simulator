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
** There are special exceptions to the terms and conditions of the
** license as they are applied to this software. View the full text of
** the exceptions in file COPYING.txt in the directory of this software
** distribution
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/

#include <yuni/yuni.h>
#include <yuni/string.h>
#include "../cleaner.h"
#include "versions.h"
#include <antares/logs/logs.h>

using namespace Yuni;

#define STUDY_CLEANER_LOG "[study cleaner] "

namespace Antares::Data
{
StudyCleaningInfos::StudyCleaningInfos()
{
    version = versionUnknown;
}

StudyCleaningInfos::StudyCleaningInfos(const AnyString& path) : folder(path)
{
    version = versionUnknown;
}

StudyCleaningInfos::~StudyCleaningInfos() = default;

bool StudyCleaningInfos::analyze()
{
    logs.info() << "cleaning study: " << folder;

    // clear
    exclude.clear();
    intruders.clear();
    postExclude.clear();
    // Getting the version
    version = StudyTryToFindTheVersion(folder);

    switch (version)
    {
    case versionFutur:
    {
        logs.error() << "A more recent version of Antares is required for " << folder;
        break;
    }
    case versionUnknown:
    {
        logs.error() << "Unknown study version: " << folder;
        break;
    }
    default:
    {
        if ((int)version <= (int)versionLatest)
        {
            if (not listOfFilesAnDirectoriesToKeep(this))
            {
                logs.error() << "Aborting: an error has been encountered: " << folder;
                return false;
            }
        }
        else
            logs.error() << "Invalid study version: " << folder;
        break;
    }
    }

    // Grab all intruders at once
    intruders.onProgress = onProgress;
    intruders.addFromFolder(folder, exclude);
    // Post cleanup in the list
    intruders.remove(postExclude);

    // Result
    if (not intruders.empty())
    {
        if (intruders.size() == 1)
            logs.info() << "  :: 1 file/folder will be removed";
        else
            logs.info() << "  :: " << intruders.size() << " files/folders will be removed";
        return true;
    }
    else
    {
        logs.info() << "  :: no file will be removed";
        return false;
    }
    return (intruders.size()) ? true : false;
}

void StudyCleaningInfos::performCleanup()
{
    if (version != versionUnknown && not intruders.empty())
    {
        // Remove all files first
        intruders.deleteAllFiles(folder);
        // Remove all folders next
        intruders.deleteAllEmptyFolders(folder);
    }
}

uint64_t StudyCleaningInfos::totalSize() const
{
    return intruders.totalSizeInBytes();
}

void StudyCleaningInfos::setCustomExcludeList(const Yuni::String& c)
{
    customExclude = c;
}
} // namespace Antares::Data

