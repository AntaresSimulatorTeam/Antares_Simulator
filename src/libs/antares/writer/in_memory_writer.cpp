// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include <algorithm>
#include <fstream>
#include <iterator>

#include <antares/benchmarking/DurationCollector.h>
#include <antares/benchmarking/timer.h>
#include <antares/io/file.h>
#include <antares/logs/logs.h>
#include <antares/writer/in_memory_writer.h>

namespace fs = std::filesystem;

#include <antares/benchmarking/DurationCollector.h>
#include <antares/benchmarking/timer.h>
#include <antares/logs/logs.h>
#include <antares/writer/in_memory_writer.h>

namespace Antares::Solver
{

namespace
{

template<class ContentT>
void addToMap(InMemoryWriter::MapType& entries,
              const std::string& entryPath,
              ContentT& content,
              std::mutex& mutex,
              Benchmarking::DurationCollector& duration_collector)
{
    std::string entryPathSanitized = entryPath;
    std::replace(entryPathSanitized.begin(), entryPathSanitized.end(), '\\', '/');
    std::unique_lock lock(mutex, std::defer_lock);
    duration_collector("in_memory_wait") << [&lock] { lock.lock(); };

    duration_collector("in_memory_insert")
      << [&] { entries.insert({entryPathSanitized, content}); };
}
} // namespace

InMemoryWriter::InMemoryWriter(Benchmarking::DurationCollector& duration_collector):
    pDurationCollector(duration_collector)
{
}

InMemoryWriter::~InMemoryWriter() = default;

void InMemoryWriter::addEntryFromBuffer(const fs::path& entryPath, std::string& entryContent)
{
    addToMap(pEntries, entryPath.string(), entryContent, pMapMutex, pDurationCollector);
}

void InMemoryWriter::addEntryFromFile(const fs::path& entryPath, const fs::path& filePath)
{
    // Shamelessly copy-pasted from zip_writer.cpp
    // TODO refactor
    std::string buffer = IO::readFile(filePath);

    addToMap(pEntries, entryPath.string(), buffer, pMapMutex, pDurationCollector);
}

void InMemoryWriter::flush()
{
    // Nothing to do here
}

bool InMemoryWriter::needsTheJobQueue() const
{
    // We may reconsider if performance is atrocious
    return false;
}

void InMemoryWriter::finalize(bool /* verbose */)
{
    // Nothing to do here
}

const InMemoryWriter::MapType& InMemoryWriter::getMap() const
{
    return pEntries;
}

} // namespace Antares::Solver
