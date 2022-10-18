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

#include "memory.h"
#include <yuni/io/directory.h>
#include <yuni/core/system/windows.hdr.h>
#include <yuni/core/system/environment.h>
#include <yuni/io/directory/info.h>
#include <yuni/core/system/windows.hdr.h>
#include "../logs.h"
#ifndef YUNI_OS_WINDOWS
#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <errno.h>
// man 2 kill
#include <sys/types.h>
#include <signal.h>
#endif
#ifdef YUNI_OS_WINDOWS
#include <WinIoCtl.h>
#endif
#include "../study/memory-usage.h"
#include "../sys/policy.h"
#include <unordered_map>

using namespace Yuni;

#define SEP Yuni::IO::Separator

namespace Antares
{
/*extern*/ Memory memory;

namespace // anonymous
{
// Global mutex for memory handler
static Yuni::Mutex gMutex;
} // anonymous namespace

Memory::Memory() : pAlreadyInitialized(false)
{
    // Nothing must be called here. There is a circular
    // reference issue when initializing global variables.
}

Memory::~Memory()
{
}

bool Memory::initializeTemporaryFolder()
{
    Yuni::MutexLocker locker(gMutex);
    if (pAlreadyInitialized)
        return true;

    pAlreadyInitialized = true;
    pAllowedToChangeCacheFolder = LocalPolicy::ReadAsBool("allow_custom_cache_folder", true);

    // Reading information from the local policy
    LocalPolicy::Read(pCacheFolder, "default_cache_folder");

    // Looking for the temporary folder if the cache folder is not set
    if (pCacheFolder.empty())
    {
        // Cache folder
        if (System::unix)
        {
            System::Environment::Read("TMPDIR", pCacheFolder);
            if (pCacheFolder.empty())
            {
                System::Environment::Read("TEMP", pCacheFolder);
                if (pCacheFolder.empty())
                {
                    System::Environment::Read("TEMPDIR", pCacheFolder);
                    if (pCacheFolder.empty())
                        pCacheFolder = "/tmp";
                }
            }
        }
        else
        {
            // Reading the 'TEMP' variable
            System::Environment::Read("TEMP", pCacheFolder);
            if (pCacheFolder.empty())
            {
                System::Environment::Read("TMP", pCacheFolder);
                if (pCacheFolder.empty())
                    pCacheFolder = "C:";
            }
        }
    }

    // Trying to create the folder
    if (not pCacheFolder.empty() and not pAllowedToChangeCacheFolder
        and not IO::Directory::Create(pCacheFolder))
    {
        // The swap support is disabled.
        logs.warning() << "Impossible to create the cache folder "
                       << pCacheFolder;
        pCacheFolder.clear();
    }
    return true;
}

void Memory::EstimateMemoryUsage(size_t bytes,
                                 uint count,
                                 Data::StudyMemoryUsage& u,
                                 bool duplicateForParallelYears)
{
    size_t total = bytes * count;
    if (duplicateForParallelYears)
        total = total * u.nbYearsParallel;

    if (u.gatheringInformationsForInput)
        u.requiredMemoryForInput += total;
    else
        u.requiredMemoryForOutput += total;
}

void Memory::displayInfo() const
{
#ifdef YUNI_OS_WINDOWS
    SYSTEM_INFO info;
    GetSystemInfo(&info);
    logs.info() << "  memory pool: system info: page size: " << info.dwPageSize
                << ", granularity: " << info.dwAllocationGranularity;
#else
    logs.info() << "  memory pool: system info: page size: " << sysconf(_SC_PAGESIZE);
#endif

    Yuni::MutexLocker locker(gMutex);
    logs.info() << "  memory pool: swap folder: " << pCacheFolder;
}

const String& Memory::cacheFolder() const
{
    MutexLocker locker(gMutex);
    return pCacheFolder;
}

void Memory::cacheFolder(const AnyString& folder)
{
    MutexLocker locker(gMutex);
    if (pAllowedToChangeCacheFolder)
        pCacheFolder = folder;
}

} // namespace Antares
