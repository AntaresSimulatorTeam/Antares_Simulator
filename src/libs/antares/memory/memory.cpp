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
#include "swapfile.h"
#include "../sys/policy.h"
#include <unordered_map>

using namespace Yuni;

#define SEP Yuni::IO::Separator

namespace Antares
{
/*extern*/ Memory memory;

namespace // anonymous
{
//! Information about a single swap file
using SwapFileInfo = Antares::Private::Memory::SwapFileInfo;
//! Informations about a list of swap files
using SwapFileList = std::vector<SwapFileInfo*>;
//! handle mapping
using MappingMap = std::unordered_map<Memory::Handle, Memory::Mapping*>;

// Global mutex for memory handler
static Yuni::Mutex gMutex;

//! The next handle which should be used
static Memory::Handle pNextHandle = 0; // It will be incremented by internalAllocate()
//! Mapping handle
static MappingMap pMapping;
//! All swap files
static SwapFileList pSwapFile;

static inline void FlushMappingWL(Memory::Mapping& mapping)
{
    void** p = const_cast<void**>(mapping.pointer);
    if (p)
    {
        if (*p)
        {
// Unmapping
#ifdef YUNI_OS_WINDOWS
            UnmapViewOfFile(*p);
#else
            munmap(*p, (mapping.nbBlocks * Memory::blockSize));
#endif
            *p = nullptr;
        }
        mapping.pointer = nullptr;
    }
}

} // anonymous namespace

Memory::Memory() : pAlreadyInitialized(false)
{
    // Nothing must be called here. There is a circular
    // reference issue when initializing global variables.
}

Memory::~Memory()
{
    // Destroying all segments
    if (not pSwapFile.empty())
    {
        // Making sure that all blocks have been unmapped
        releaseAll();
        // Destroying all swap files
        for (uint i = 0; i != pSwapFile.size(); ++i)
            delete pSwapFile[i];
    }
}

bool Memory::initialize()
{
    Yuni::MutexLocker locker(gMutex);
    if (pAlreadyInitialized)
        return true;

    pAlreadyInitialized = true;
    pAllowedToChangeCacheFolder = LocalPolicy::ReadAsBool("allow_custom_swap_folder", true);
    waitForSwapFileDeletion = false;

    // pMapping.set_empty_key(0);
    // pMapping.set_deleted_key((uint) -1);

    // Reading information from the local policy
    LocalPolicy::Read(pCacheFolder, "default_swap_folder");

    // swap file names
    // see prefix.cpp
    initializeSwapFilePrefix();

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
        logs.warning() << "Swap support is disabled. Impossible to create the folder "
                       << pCacheFolder;
        pCacheFolder.clear();
    }
    return true;
}

void Memory::removeAllUnusedSwapFiles()
{
    Yuni::MutexLocker locker(gMutex);
    assert(pAlreadyInitialized and "swap memory not initialized");

    // Destroying all segments
    if (not pSwapFile.empty())
    {
        // Flush all handle first
        {
            const MappingMap::iterator end = pMapping.end();
            for (MappingMap::iterator i = pMapping.begin(); i != end; ++i)
                FlushMappingWL(*(i->second));
        }

        bool stop = true;
        do
        {
            stop = true;
            SwapFileList::iterator end = pSwapFile.end();
            for (SwapFileList::iterator i = pSwapFile.begin(); i != end; ++i)
            {
                delete *i;
                pSwapFile.erase(i);
                stop = false;
                break;
            }
        } while (!stop);
    }
}

void Memory::dumpSwapFilesInfos() const
{
    Yuni::MutexLocker locker(gMutex);
    dumpSwapFilesInfosWL();
}

void Memory::dumpSwapFilesInfosWL() const
{
    logs.info() << "[memory][summary] -- begin";
    // Destroying all segments
    if (not pSwapFile.empty())
    {
        uint index = 0;
        const SwapFileList::const_iterator end = pSwapFile.end();
        for (SwapFileList::const_iterator i = pSwapFile.begin(); i != end; ++i)
        {
            const SwapFileInfo& s = *(*i);
            logs.info() << "[memory][summary] -- swap " << index << ": "
                        << (((blockPerSwap - s.nbFreeBlocks) * blockSize) / 1024 / 1024) << "Mo / "
                        << (swapSize / 1024 / 1024) << "Mo, " << s.nbFreeBlocks << " free blocks";
            ++index;
        }
    }
    else
        logs.info() << "[memory][summary] No swap file available";
    logs.info() << "[memory][summary] -- end";
}

bool Memory::createNewSwapFileWL()
{
    // We have detected there is not enough disk space. It is useless to
    // continue.
    if (not pCacheFolder or waitForSwapFileDeletion)
        return false;

    SwapFileInfo* info = new SwapFileInfo();

    // The swap filename
    info->filename << pCacheFolder << SEP << pSwapFilePrefix << pSwapFilePrefixProcessID
                   << (int)pSwapFile.size() << ".antares-swap";

    if (!info->openSwapFile((uint)pSwapFile.size(), false))
    {
        delete info;
        waitForSwapFileDeletion = true;
        return false;
    }
    // Adding it into the swap list
    pSwapFile.push_back(info);
    return true;
}

uint64 Memory::memoryUsage() const
{
    uint64 result = sizeof(Memory);

    Yuni::MutexLocker locker(gMutex);
    result += pCacheFolder.capacity();
    result += pSwapFilePrefix.capacity();
    result += pSwapFile.size() * (sizeof(void*) * 2); // overhead std::map - estimation

    for (uint i = 0; i != pSwapFile.size(); ++i)
    {
        const SwapFileInfo& swap = *(pSwapFile[i]);
        result += sizeof(SwapFileInfo) + swap.filename.capacity() + swap.blocks.size();
    }

    // mapping overhead
    // (sizeof(A) + sizeof(B) + ELEMENT_OVERHEAD) * N + CONTAINER_OVERHEAD
    enum
    {
        mapOverheadPerElement
        = (sizeof(Handle) + sizeof(Mapping*) + sizeof(32 /*std::_Rb_tree_node_base*/))
    };
    result += pMapping.size() * (sizeof(Mapping) + mapOverheadPerElement);

    const MappingMap::const_iterator end = pMapping.end();
    for (MappingMap::const_iterator i = pMapping.begin(); i != end; ++i)
    {
        const Mapping& mapping = *(i->second);
        if (mapping.pointer)
            result += mapping.nbBlocks * blockSize;
    }
    return result;
}


Memory::Handle Memory::internalAllocate(size_t size)
{
    // This allocator does not suit well for small chunks
    if (size < minimalAllocationSize)
        return invalidHandle;

    enum
    {
        npos = Bit::Array::npos
    };

    // looking for the total number of blocks required for the allocation
    uint blockNeeded = 0;
    do
    {
        // We use a 'do...while' here to properly handle null sizes.
        ++blockNeeded;
        size -= ((size > (size_t)blockSize) ? (size_t)blockSize : size);
    } while (size);

    Yuni::MutexLocker locker(gMutex);
    assert(pAlreadyInitialized and "swap memory not initialized");
    do
    {
        // Browsing all available swap file to find an empty slot
        // We will browse them in the reverse because the free memory
        // should be into the last one.
        uint i = (uint)pSwapFile.size();
        while (i--)
        {
            // Information about the current swap file
            SwapFileInfo& swap = *(pSwapFile[i]);

            // Asserts
            assert(swap.nbFreeBlocks <= blockPerSwap);
            assert(swap.filename.size() < swap.filename.capacity());

            // trivial check
            // It is useless to continue further if there is not enough block in this swap
            if (swap.nbFreeBlocks < blockNeeded)
                continue;

            // Alias to the bitmap index
            Bit::Array& bitmap = swap.blocks;
            // Looking for the first free block
            uint offset;
            if (npos == (offset = bitmap.findN<false>(blockNeeded, swap.lastOffset)))
            {
                // new attempt from the begining, which is however not needed
                // if the last offset was null
                if (!swap.lastOffset or npos == (offset = bitmap.findN<false>(blockNeeded, 0)))
                {
                    // impossible to find a suitable free block in this swap file. Aborting.
                    continue;
                }
            }

            // updating the last offset reminder
            // We should have more luck the next time to allocate a new block from this position
            swap.lastOffset = offset + blockNeeded;

            // We have found our free blocks !
            // Flagging that the blocks are already taken
            for (uint j = 0; j != blockNeeded; ++j)
                bitmap.set(offset + j);

            // Looking for a valid handle
            {
                const MappingMap::iterator end = pMapping.end();
                while (pMapping.find(++pNextHandle) != end
                       or not pNextHandle) // or (uint) -1 == pNextHandle)
                {
                    // does nothing
                }
            }

            // Less blocks are now available
            swap.nbFreeBlocks -= blockNeeded;

            // Registering the new handle
            Mapping* mapping = new Mapping();
            mapping->pointer = nullptr;
            mapping->offset = offset;
            mapping->nbBlocks = blockNeeded;
            mapping->swapFile = &swap;
            pMapping[pNextHandle] = mapping;
            // std::cout << "  allocate handle " << pNextHandle << " of " << blockNeeded << "
            // blocks" << '\n';

            // Returning the current handle
            return pNextHandle;
        }

        // No free block remains. We have to allocate a new swap file
        // and try again.
        if (not createNewSwapFileWL())
            return invalidHandle;
    } while (true);

    return invalidHandle;
}

void Memory::releaseAll()
{
    Yuni::MutexLocker locker(gMutex);

    // higher pointers should be located into the last swap files
    MappingMap::iterator end = pMapping.end();
    for (MappingMap::iterator i = pMapping.begin(); i != end; ++i)
    {
        Mapping& mapping = *(i->second);
        releaseWL(mapping);
        delete i->second;
    }
    pMapping.clear();

    // Reset the next handle
    pNextHandle = 0;
}

void Memory::releaseWL(Mapping& mapping)
{
    // alias to the current swap file
    SwapFileInfo& currentSwapFile = *mapping.swapFile;
    // Updating the new amount of free blocks
    currentSwapFile.nbFreeBlocks += mapping.nbBlocks;

    // unmapping the pointer
    // On Windows, all pointers must be unmapped before closing the file
    // descriptor
    // This is not mandatory on Unixes but it will be better like that.
    void** p = const_cast<void**>(mapping.pointer);
    if (p and *p)
    {
// Unmapping
#ifdef YUNI_OS_WINDOWS
        UnmapViewOfFile(*p);
#else
        munmap(*p, mapping.nbBlocks * blockSize);
#endif
        *p = nullptr;
    }

    // Checking if we should merely remove the swap file
    if (currentSwapFile.nbFreeBlocks == blockPerSwap and pSwapFile.size() != 1)
    {
        // Removing it from the list
        // We suppose that the most recent pointers will be deleted first, which
        // should be mostly true
        {
            const SwapFileInfo* lookup = mapping.swapFile;
            uint i = (uint)pSwapFile.size();
            do
            {
                if (lookup == pSwapFile[--i])
                {
                    pSwapFile.erase(pSwapFile.begin() + i);
                    break;
                }
            } while (i != 0);
        }

        // logging
        uint64 newSize = ((uint64)pSwapFile.size() * ::Antares::Memory::swapSize) / (1024 * 1024);
        logs.info() << "  memory pool: shrinking to " << newSize << "Mo";
        // Destroying for real the swap file
        // As the number of free blocks is equal to blockPerSwap, we assume that
        // all block had been unmapped
        delete mapping.swapFile;
        mapping.swapFile = nullptr;
        // We may have some disk space now
        waitForSwapFileDeletion = false;
    }
    else
    {
        // Releasing the memory region
        Bit::Array& bitmap = currentSwapFile.blocks;
        for (uint j = 0; j != mapping.nbBlocks; ++j)
            bitmap.unset(mapping.offset + j);
    }
}

void Memory::release(Memory::Handle handle)
{
    assert(handle != 0);
    gMutex.lock();

    MappingMap::iterator i = pMapping.find(handle);
    if (i != pMapping.end())
    {
        Mapping* mapping = i->second;
        releaseWL(*mapping);
        pMapping.erase(i);
        gMutex.unlock();

        delete mapping;
    }
    else
        gMutex.unlock();
}

void* Memory::acquireMapping(Handle handle, volatile void* pointer)
{
    assert(handle != 0);
    assert(pointer != NULL);

    // Locking
    Yuni::MutexLocker locker(gMutex);

    MappingMap::iterator i = pMapping.find(handle);
    if (i != pMapping.end())
    {
        Mapping& m = *(i->second);
        m.pointer = (volatile void**)pointer;

// Unmapping
#ifdef YUNI_OS_WINDOWS
        // On Windows, for code safety, we have to use exceptions....
        try
        {
            LARGE_INTEGER offset;
            offset.QuadPart = m.offset * blockSize;
            void* p = MapViewOfFile(m.swapFile->mappingHandle,
                                    FILE_MAP_ALL_ACCESS,
                                    offset.HighPart,
                                    offset.LowPart,
                                    m.nbBlocks * blockSize);
            if (p == nullptr)
            {
                DWORD error = GetLastError();
                if (error == 8) // Not enough memory
                {
                    logs.error() << "memory pool: impossible to access to the swap file : "
                                 << m.swapFile->filename << " (offset: " << m.offset
                                 << ", Not enough memory)";
                }
                else
                {
                    logs.error() << "memory pool: impossible to access to the swap file : "
                                 << m.swapFile->filename << " (offset: " << (sint64)m.offset
                                 << ", err: " << error << ")";
                }
                return nullptr;
            }
            return p;
        }
        catch (...)
        {
            logs.error() << "memory pool: impossible to access to the swap file : "
                         << m.swapFile->filename << " (offset: " << (sint64)m.offset << ')';
        }
        return nullptr;

#else

        void* p = mmap(0,
                       m.nbBlocks * blockSize,
                       PROT_READ | PROT_WRITE,
                       MAP_SHARED,
                       m.swapFile->handle,
                       m.offset * blockSize);
        if (p == MAP_FAILED)
        {
            logs.error() << "memory pool: impossible to access to the swap file : "
                         << m.swapFile->filename;
            return nullptr;
        }
        return p;
#endif
    }

    logs.error() << "memory pool: impossible to acquire the ressource " << handle
                 << ": Resource ID not found";
#ifndef NDEBUG
    {
        logs.info() << "trace: ";
        const MappingMap::iterator end = pMapping.end();
        for (MappingMap::iterator i = pMapping.begin(); i != end; ++i)
        {
            logs.info() << "  id: " << i->first << ",  ptr: " << (void*)i->second->pointer
                        << ",  offset: " << i->second->offset;
        }
    }
#endif

    return nullptr;
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

void Memory::cleanupCacheFolder() const
{
    // Copy
    String folder;
    String prefix;
    uint64 ourProcessID;
    {
        // Locking
        Yuni::MutexLocker locker(gMutex);
        folder = pCacheFolder;
        prefix = pSwapFilePrefix;
        ourProcessID = pProcessID;
    }

    // List of files to delete
    // We will delay the deletion of the swap files to make sure that
    // they are released by the system (Windows)
    using DeleteList = std::vector<String>;
    DeleteList toDelete;

    {
        // Note: the variable is already used on Windoes...
        IO::Directory::Info dirinfo(folder);
        String s;

        // Building the file list scheduled for deletion
        const auto end = dirinfo.file_end();
        for (auto i = dirinfo.file_begin(); i != end; ++i)
        {
            const String& name = *i;
            if (name.startsWith(prefix))
            {
                // retrieving the pid from the filename
                s = name;
                s.erase(0, prefix.size());
                uint index = s.find('-');
                if (index != String::npos)
                    s.truncate(index);
                if (!s)
                    continue;

                uint64 pid;
                pid = s.to<uint64>();
                // Checking for an invalid pid or if the pid is ourselves
                if (pid <= 65535 and pid != ourProcessID)
                {
#ifndef YUNI_OS_WINDOWS
                    // UNIX
                    // Checking if the process is still alive
                    if (kill(pid, 0))
                    {
                        switch (errno)
                        {
                        case EPERM:
                            logs.error() << "We don't have enough privileges to send a signal";
                            break;
                        case ESRCH:
                            toDelete.push_back(i.filename());
                            break;
                        }
                    }
#else
                    toDelete.push_back(i.filename());
#endif
                }
            }
        }
    }

    if (not toDelete.empty())
    {
        const DeleteList::const_iterator end = toDelete.end();
        DeleteList::const_iterator i = toDelete.begin();
        for (; i != end; ++i)
        {
            // Try to delete the swap file
            if (not IO::File::Delete(*i))
            {
                // The file may already have been deleted
                if (IO::Exists(*i))
                    logs.error() << "  memory pool: impossible to delete " << *i;
            }
            else
                logs.info() << "  memory pool: removing unused swap file " << *i;
        }
    }
}

void Memory::ensureOneSwapFile()
{
    MutexLocker locker(gMutex);
    if (pSwapFile.empty())
        createNewSwapFileWL();
}

uint64 Memory::memoryCapacity() const
{
    MutexLocker locker(gMutex);
    return pSwapFile.size() * swapSize;
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
