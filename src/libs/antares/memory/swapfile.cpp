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

#include "swapfile.h"
#include <yuni/io/directory.h>
#include <yuni/core/system/windows.hdr.h>
#include <yuni/core/system/environment.h>
#include <yuni/io/directory/info.h>
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
#include <sys/statvfs.h>
#endif
#ifdef YUNI_OS_WINDOWS
#include <yuni/core/string/wstring.h>
#include <WinIoCtl.h>
#endif
#include "../study/memory-usage.h"

#define SEP Yuni::IO::Separator

#define LOG_SWAP_ERR_POSSIBLE_CAUSES                                                         \
    "The swap support is disabled. Possible causes: Not enough free disk space, not enough " \
    "permissions or swap folder on a network drive"

#define LOG_SWAP_NOT_ENOUGH_DISK_SPACE                                                         \
    logs.warning() << "impossible to allocate " << (::Antares::Memory::swapSize / 1024 / 1024) \
                   << " Mo on disk. " << LOG_SWAP_ERR_POSSIBLE_CAUSES

#define LOG_SWAP_NOT_ENOUGH_DISK_SPACE_RETURN_FALSE                                                \
    do                                                                                             \
    {                                                                                              \
        logs.warning() << "impossible to allocate " << (::Antares::Memory::swapSize / 1024 / 1024) \
                       << " Mo on disk. " << LOG_SWAP_ERR_POSSIBLE_CAUSES;                         \
        return false;                                                                              \
    } while (0)

using namespace Yuni;

namespace Antares
{
namespace Private
{
namespace Memory
{
SwapFileInfo::SwapFileInfo() : lastOffset(0), nbFreeBlocks(Antares::Memory::blockPerSwap)
{
    blocks.resize(Antares::Memory::blockPerSwap);
    blocks.unset();
}

SwapFileInfo::~SwapFileInfo()
{
    if (Yuni::Logs::Verbosity::Debug::enabled)
        logs.debug() << "memory pool: destroying " << filename;

#ifdef YUNI_OS_WINDOWS
    if (mappingHandle != INVALID_HANDLE_VALUE)
        CloseHandle(mappingHandle);
    if (handle != INVALID_HANDLE_VALUE)
        CloseHandle(handle);
#else

    // Closing the file
    if (handle != -1)
        ::close(handle);
#endif

    if (IO::errNone != IO::File::Delete(filename))
    {
        // Impossible to unlink. Checking if the file still exists
        // before throwing an error
        // if (IO::File::Exists(filename))
        //	logs.error() << "memory pool: impossible to remove " << filename;
    }
}

  bool SwapFileInfo::openSwapFile(uint count, bool displayDiskLogs)
{
    if (displayDiskLogs)
        logs.info() << "Reserving disk space...";

    {
        uint64 size = (((uint64)(++count) * (uint64)::Antares::Memory::swapSize) / 1024u / 1024u);
        logs.info() << "  memory pool: expanding to " << size << "Mo. Reserving disk space in "
                    << filename;
    }

#ifdef YUNI_OS_WINDOWS

    // reset
    mappingHandle = INVALID_HANDLE_VALUE;
    handle = INVALID_HANDLE_VALUE;

    // Open the file
    {
        Yuni::WString buffer(filename);
        if (buffer.empty())
            return false;
        handle = CreateFileW(buffer.c_str(),
                             GENERIC_READ | GENERIC_WRITE,
                             FILE_SHARE_WRITE | FILE_SHARE_READ,
                             nullptr,
                             CREATE_ALWAYS,
                             FILE_ATTRIBUTE_NORMAL,
                             nullptr);
    }

    if (handle != INVALID_HANDLE_VALUE)
    {
        // Creating the chunk
        // On Windows, it seems we need to add some extra space
        // Otherwise we won't be able to allocate the last block
        ::SetFilePointer(
          handle, ::Antares::Memory::swapSize + ::Antares::Memory::blockSize, 0, FILE_BEGIN);
        if (::SetEndOfFile(handle))
        {
            FILE_ZERO_DATA_INFORMATION fzdi;
            DWORD dwTemp;
            fzdi.FileOffset.QuadPart = 0;
            fzdi.BeyondFinalZero.QuadPart
              = ::Antares::Memory::swapSize + ::Antares::Memory::blockSize;
            // Write all zeros !
            if (!::DeviceIoControl(
                  handle, FSCTL_SET_ZERO_DATA, &fzdi, sizeof(fzdi), nullptr, 0, &dwTemp, nullptr))
                LOG_SWAP_NOT_ENOUGH_DISK_SPACE_RETURN_FALSE;

            // Mapping
            LARGE_INTEGER offset;
            offset.QuadPart = Antares::Memory::swapSize;
            mappingHandle = CreateFileMapping(
              handle, nullptr, PAGE_READWRITE, offset.HighPart, offset.LowPart, nullptr);
            return (mappingHandle != INVALID_HANDLE_VALUE);
        }
        else
            LOG_SWAP_NOT_ENOUGH_DISK_SPACE_RETURN_FALSE;
    }
    else
    {
        logs.warning() << "[swap] Impossible to create a new swap file. Not enough free disk space "
                          "or not enough permissions";
    }

    return false;

#else

// Opening the file
#ifndef YUNI_OS_MACOS
    int flags = O_RDWR | O_CREAT | O_TRUNC | O_NOATIME;
#else
    int flags = O_RDWR | O_CREAT | O_TRUNC; // O_NOATIME not supported on OS X
#endif
    if (-1 == (handle = open(filename.c_str(), flags, (mode_t)0600)))
    {
        logs.warning() << "[swap] Impossible to create a new swap file. Not enough free disk space "
                          "or not enough permissions";
        return false;
    }
    if (::ftruncate(handle, (off_t)::Antares::Memory::swapSize) != 0)
        LOG_SWAP_NOT_ENOUGH_DISK_SPACE_RETURN_FALSE;
    if ((off_t)lseek(handle, 0, SEEK_SET) < 0)
        LOG_SWAP_NOT_ENOUGH_DISK_SPACE_RETURN_FALSE;

    struct statvfs fsinfo;
    if (!fstatvfs(handle, &fsinfo))
    {
        if ((uint64)fsinfo.f_bsize * fsinfo.f_bavail > 4 * (uint64)::Antares::Memory::swapSize)
        {
            // Ok ! We have enough space for allocating the swap file
            return true;
        }
    }
    // You should be sure that we can allocate the swap file
    {
        enum
        {
            bufferSize = 1024 * 1024
        }; // 1Mo
        char* emptyBuffer = new char[bufferSize];

        (void)::memset(emptyBuffer, 0, bufferSize);

        for (uint i = 0; i != ::Antares::Memory::swapSize; i += bufferSize)
        {
            if (::write(handle, emptyBuffer, bufferSize) != (ssize_t)bufferSize)
                LOG_SWAP_NOT_ENOUGH_DISK_SPACE_RETURN_FALSE;
        }
        delete[] emptyBuffer;
    }
    return true;

#endif
}

} // namespace Memory
} // namespace Private
} // namespace Antares
