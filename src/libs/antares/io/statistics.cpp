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

#include <atomic>
#include "antares/io/statistics.h"
#include <antares/logs/logs.h>

using namespace Yuni;

namespace Antares
{
namespace Statistics
{
namespace // anonymous namespace
{
static std::atomic<uint64_t> gReadFromDisk;
static std::atomic<uint64_t> gWrittenToDisk;
static std::atomic<uint64_t> gReadFromNetwork;
static std::atomic<uint64_t> gWrittenToNetwork;

static std::atomic<uint64_t> gReadFromDiskSinceStartup;
static std::atomic<uint64_t> gWrittenToDiskSinceStartup;
static std::atomic<uint64_t> gReadFromNetworkSinceStartup;
static std::atomic<uint64_t> gWrittenToNetworkSinceStartup;

} // anonymous namespace

uint64_t ReadFromDisk()
{
    return gReadFromDisk;
}

uint64_t WrittenToDisk()
{
    return gWrittenToDisk;
}

uint64_t ReadFromDiskSinceStartup()
{
    return gReadFromDiskSinceStartup;
}

uint64_t WrittenToDiskSinceStartup()
{
    return (int64_t)gWrittenToDiskSinceStartup;
}

uint64_t ReadFromNetwork()
{
    return gReadFromNetwork;
}

uint64_t WrittenToNetwork()
{
    return gWrittenToNetwork;
}

uint64_t ReadFromNetworkSinceStartup()
{
    return gReadFromNetworkSinceStartup;
}

uint64_t WrittenToNetworkSinceStartup()
{
    return gWrittenToNetworkSinceStartup;
}

void Reset()
{
    gReadFromDisk = 0;
    gReadFromNetwork = 0;
    gWrittenToDisk = 0;
    gWrittenToNetwork = 0;
}

void DumpToLogs()
{
    logs.info() << "[statistics] disk: read: " << (gReadFromDisk / 1024)
                << " ko, written: " << (gWrittenToDisk / 1024) << " ko";
    logs.info() << "[statistics] network: read: " << (gReadFromNetwork / 1024)
                << " ko, written: " << (gWrittenToNetwork / 1024) << " ko";
}

void HasReadFromDisk(uint64_t size)
{
    gReadFromDisk += size;
    gReadFromDiskSinceStartup += size;
}

void HasWrittenToDisk(uint64_t size)
{
    gWrittenToDisk += size;
    gWrittenToDiskSinceStartup += size;
}

void HasReadFromNetwork(uint64_t size)
{
    gReadFromNetwork += size;
    gReadFromNetworkSinceStartup += size;
}

void HasWrittenToNetwork(uint64_t size)
{
    gWrittenToNetwork += size;
    gWrittenToNetworkSinceStartup += size;
}

} // namespace Statistics
} // namespace Antares
