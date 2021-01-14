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

#include "statistics.h"
#include <yuni/core/atomic/int.h>
#include "../logs.h"

using namespace Yuni;

namespace Antares
{
namespace Statistics
{
namespace // anonymous namespace
{
static uint64 gReadFromDisk;
static uint64 gWrittenToDisk;
static uint64 gReadFromNetwork;
static uint64 gWrittenToNetwork;

static uint64 gReadFromDiskSinceStartup;
static uint64 gWrittenToDiskSinceStartup;
static uint64 gReadFromNetworkSinceStartup;
static uint64 gWrittenToNetworkSinceStartup;

static Yuni::Mutex gMutex;

} // anonymous namespace

uint64 ReadFromDisk()
{
    Yuni::MutexLocker locker(gMutex);
    return (uint64)gReadFromDisk;
}

uint64 WrittenToDisk()
{
    Yuni::MutexLocker locker(gMutex);
    return (uint64)gWrittenToDisk;
}

uint64 ReadFromDiskSinceStartup()
{
    Yuni::MutexLocker locker(gMutex);
    return (uint64)gReadFromDiskSinceStartup;
}

uint64 WrittenToDiskSinceStartup()
{
    Yuni::MutexLocker locker(gMutex);
    return (uint64)((sint64)gWrittenToDiskSinceStartup);
}

uint64 ReadFromNetwork()
{
    Yuni::MutexLocker locker(gMutex);
    return (uint64)gReadFromNetwork;
}

uint64 WrittenToNetwork()
{
    Yuni::MutexLocker locker(gMutex);
    return (uint64)gWrittenToNetwork;
}

uint64 ReadFromNetworkSinceStartup()
{
    Yuni::MutexLocker locker(gMutex);
    return (uint64)gReadFromNetworkSinceStartup;
}

uint64 WrittenToNetworkSinceStartup()
{
    Yuni::MutexLocker locker(gMutex);
    return (uint64)gWrittenToNetworkSinceStartup;
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

void HasReadFromDisk(Yuni::uint64 size)
{
    Yuni::MutexLocker locker(gMutex);
    gReadFromDisk += size;
    gReadFromDiskSinceStartup += size;
}

void HasWrittenToDisk(Yuni::uint64 size)
{
    Yuni::MutexLocker locker(gMutex);
    gWrittenToDisk += size;
    gWrittenToDiskSinceStartup += size;
}

void HasReadFromNetwork(Yuni::uint64 size)
{
    Yuni::MutexLocker locker(gMutex);
    gReadFromNetwork += size;
    gReadFromNetworkSinceStartup += size;
}

void HasWrittenToNetwork(Yuni::uint64 size)
{
    Yuni::MutexLocker locker(gMutex);
    gWrittenToNetwork += size;
    gWrittenToNetworkSinceStartup += size;
}

} // namespace Statistics
} // namespace Antares
