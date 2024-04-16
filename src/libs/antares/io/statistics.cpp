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

#include "antares/io/statistics.h"

#include <mutex>

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

static std::mutex gMutex;

} // anonymous namespace

uint64_t
ReadFromDisk()
{
    std::lock_guard locker(gMutex);
    return (uint64_t)gReadFromDisk;
}

uint64_t
WrittenToDisk()
{
    std::lock_guard locker(gMutex);
    return (uint64_t)gWrittenToDisk;
}

uint64_t
ReadFromDiskSinceStartup()
{
    std::lock_guard locker(gMutex);
    return (uint64_t)gReadFromDiskSinceStartup;
}

uint64_t
WrittenToDiskSinceStartup()
{
    std::lock_guard locker(gMutex);
    return (int64_t)gWrittenToDiskSinceStartup;
}

uint64_t
ReadFromNetwork()
{
    std::lock_guard locker(gMutex);
    return (uint64_t)gReadFromNetwork;
}

uint64_t
WrittenToNetwork()
{
    std::lock_guard locker(gMutex);
    return (uint64_t)gWrittenToNetwork;
}

uint64_t
ReadFromNetworkSinceStartup()
{
    std::lock_guard locker(gMutex);
    return (uint64_t)gReadFromNetworkSinceStartup;
}

uint64_t
WrittenToNetworkSinceStartup()
{
    std::lock_guard locker(gMutex);
    return (uint64_t)gWrittenToNetworkSinceStartup;
}

void
Reset()
{
    std::lock_guard locker(gMutex);
    gReadFromDisk = 0;
    gReadFromNetwork = 0;
    gWrittenToDisk = 0;
    gWrittenToNetwork = 0;
}

void
DumpToLogs()
{
    std::lock_guard locker(gMutex);
    logs.info() << "[statistics] disk: read: " << (gReadFromDisk / 1024)
                << " ko, written: " << (gWrittenToDisk / 1024) << " ko";
    logs.info() << "[statistics] network: read: " << (gReadFromNetwork / 1024)
                << " ko, written: " << (gWrittenToNetwork / 1024) << " ko";
}

void
HasReadFromDisk(uint64_t size)
{
    std::lock_guard locker(gMutex);
    gReadFromDisk += size;
    gReadFromDiskSinceStartup += size;
}

void
HasWrittenToDisk(uint64_t size)
{
    std::lock_guard locker(gMutex);
    gWrittenToDisk += size;
    gWrittenToDiskSinceStartup += size;
}

void
HasReadFromNetwork(uint64_t size)
{
    std::lock_guard locker(gMutex);
    gReadFromNetwork += size;
    gReadFromNetworkSinceStartup += size;
}

void
HasWrittenToNetwork(uint64_t size)
{
    std::lock_guard locker(gMutex);
    gWrittenToNetwork += size;
    gWrittenToNetworkSinceStartup += size;
}

} // namespace Statistics
} // namespace Antares
