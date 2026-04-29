// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/io/statistics.h"

#include <atomic>

#include <antares/logs/logs.h>

using namespace Yuni;

namespace Antares::Statistics
{
namespace // anonymous namespace
{
std::atomic<uint64_t> gReadFromDisk;
std::atomic<uint64_t> gWrittenToDisk;
std::atomic<uint64_t> gReadFromNetwork;
std::atomic<uint64_t> gWrittenToNetwork;

std::atomic<uint64_t> gReadFromDiskSinceStartup;
std::atomic<uint64_t> gWrittenToDiskSinceStartup;
std::atomic<uint64_t> gReadFromNetworkSinceStartup;
std::atomic<uint64_t> gWrittenToNetworkSinceStartup;

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

} // namespace Antares::Statistics
