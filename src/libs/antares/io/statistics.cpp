/*
** Copyright 2007-2023 RTE
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

#include "antares/io/statistics.h"
#include <yuni/core/atomic/int.h>
#include <antares/logs/logs.h>

using namespace Yuni;

namespace Antares
{
namespace Statistics
{
namespace // anonymous namespace
{
static uint64_t gReadFromDisk;
static uint64_t gWrittenToDisk;
static uint64_t gReadFromNetwork;
static uint64_t gWrittenToNetwork;

static uint64_t gReadFromDiskSinceStartup;
static uint64_t gWrittenToDiskSinceStartup;
static uint64_t gReadFromNetworkSinceStartup;
static uint64_t gWrittenToNetworkSinceStartup;

static Yuni::Mutex gMutex;

} // anonymous namespace

uint64_t ReadFromDisk()
{
    Yuni::MutexLocker locker(gMutex);
    return (uint64_t)gReadFromDisk;
}

uint64_t WrittenToDisk()
{
    Yuni::MutexLocker locker(gMutex);
    return (uint64_t)gWrittenToDisk;
}

uint64_t ReadFromDiskSinceStartup()
{
    Yuni::MutexLocker locker(gMutex);
    return (uint64_t)gReadFromDiskSinceStartup;
}

uint64_t WrittenToDiskSinceStartup()
{
    Yuni::MutexLocker locker(gMutex);
    return (int64_t)gWrittenToDiskSinceStartup;
}

uint64_t ReadFromNetwork()
{
    Yuni::MutexLocker locker(gMutex);
    return (uint64_t)gReadFromNetwork;
}

uint64_t WrittenToNetwork()
{
    Yuni::MutexLocker locker(gMutex);
    return (uint64_t)gWrittenToNetwork;
}

uint64_t ReadFromNetworkSinceStartup()
{
    Yuni::MutexLocker locker(gMutex);
    return (uint64_t)gReadFromNetworkSinceStartup;
}

uint64_t WrittenToNetworkSinceStartup()
{
    Yuni::MutexLocker locker(gMutex);
    return (uint64_t)gWrittenToNetworkSinceStartup;
}

void Reset()
{
    Yuni::MutexLocker locker(gMutex);
    gReadFromDisk = 0;
    gReadFromNetwork = 0;
    gWrittenToDisk = 0;
    gWrittenToNetwork = 0;
}

void DumpToLogs()
{
    Yuni::MutexLocker locker(gMutex);
    logs.info() << "[statistics] disk: read: " << (gReadFromDisk / 1024)
                << " ko, written: " << (gWrittenToDisk / 1024) << " ko";
    logs.info() << "[statistics] network: read: " << (gReadFromNetwork / 1024)
                << " ko, written: " << (gWrittenToNetwork / 1024) << " ko";
}

void HasReadFromDisk(uint64_t size)
{
    Yuni::MutexLocker locker(gMutex);
    gReadFromDisk += size;
    gReadFromDiskSinceStartup += size;
}

void HasWrittenToDisk(uint64_t size)
{
    Yuni::MutexLocker locker(gMutex);
    gWrittenToDisk += size;
    gWrittenToDiskSinceStartup += size;
}

void HasReadFromNetwork(uint64_t size)
{
    Yuni::MutexLocker locker(gMutex);
    gReadFromNetwork += size;
    gReadFromNetworkSinceStartup += size;
}

void HasWrittenToNetwork(uint64_t size)
{
    Yuni::MutexLocker locker(gMutex);
    gWrittenToNetwork += size;
    gWrittenToNetworkSinceStartup += size;
}

} // namespace Statistics
} // namespace Antares
