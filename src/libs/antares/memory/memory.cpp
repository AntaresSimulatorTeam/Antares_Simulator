// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include <mutex>

#include <yuni/core/system/environment.h>
#include <yuni/core/system/windows.hdr.h>
#include <yuni/io/directory.h>

#include <antares/logs/logs.h>
#ifndef YUNI_OS_WINDOWS
#include <cerrno>
#include <cstdio>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <unistd.h>
// man 2 kill
#include <csignal>
#include <sys/types.h>
#endif
#ifdef YUNI_OS_WINDOWS
#include <WinIoCtl.h>
#endif
#include <yuni/core/system/process.h>

#include <antares/sys/policy.h>
#include "antares/memory/memory.h"

using namespace Yuni;

namespace Antares
{
/*extern*/
Memory memory;

namespace // anonymous
{
// Global mutex for memory handler
static std::mutex gMutex;
} // anonymous namespace

bool Memory::initializeTemporaryFolder()
{
    std::lock_guard locker(gMutex);
    if (pAlreadyInitialized)
    {
        return true;
    }

    pAlreadyInitialized = true;
    pAllowedToChangeCacheFolder = LocalPolicy::ReadAsBool("allow_custom_cache_folder", true);

    // Reading information from the local policy
    LocalPolicy::Read(pCacheFolder, "default_cache_folder");

    pProcessID = Yuni::ProcessID();

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
                    {
                        pCacheFolder = "/tmp";
                    }
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
                {
                    pCacheFolder = "C:";
                }
            }
        }
    }

    // Trying to create the folder
    if (not pCacheFolder.empty() and not pAllowedToChangeCacheFolder
        and not IO::Directory::Create(pCacheFolder))
    {
        logs.warning() << "Impossible to create the cache folder " << pCacheFolder;
        pCacheFolder.clear();
    }
    return true;
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

    std::lock_guard locker(gMutex);
    logs.info() << "  memory pool: cache folder: " << pCacheFolder;
}

const String& Memory::cacheFolder() const
{
    std::lock_guard locker(gMutex);
    return pCacheFolder;
}

void Memory::cacheFolder(const AnyString& folder)
{
    std::lock_guard locker(gMutex);
    if (pAllowedToChangeCacheFolder)
    {
        pCacheFolder = folder;
    }
}

} // namespace Antares
