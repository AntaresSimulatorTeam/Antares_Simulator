// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/study/cleaner.h"




#include <antares/logs/logs.h>
#include <antares/study/header.h>
#include "antares/study/cleaner/versions.h"
#include "antares/study/version.h"


#define STUDY_CLEANER_LOG "[study cleaner] "

namespace Antares::Data
{
StudyCleaningInfos::StudyCleaningInfos()
{
}

StudyCleaningInfos::StudyCleaningInfos(const std::string& path):
    folder(path)
{
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
    version = StudyHeader::tryToFindTheVersion(folder);

    if (version > StudyVersion::latest())
    {
        logs.error() << "A more recent version of Antares is required for " << folder;
        return false;
    }
    if (version == StudyVersion::unknown())
    {
        logs.error() << "Unknown study version: " << folder;
        return false;
    }

    if (not listOfFilesAnDirectoriesToKeep(this))
    {
        logs.error() << "Aborting: an error has been encountered: " << folder;
        return false;
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
        {
            logs.info() << "  :: 1 file/folder will be removed";
        }
        else
        {
            logs.info() << "  :: " << intruders.size() << " files/folders will be removed";
        }
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
    if (version != StudyVersion::unknown() && not intruders.empty())
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

void StudyCleaningInfos::setCustomExcludeList(const std::string& c)
{
    customExclude = c;
}
} // namespace Antares::Data
