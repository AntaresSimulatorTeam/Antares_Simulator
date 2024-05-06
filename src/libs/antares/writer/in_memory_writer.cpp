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
#include <algorithm>

#include <yuni/io/file.h> // Yuni::IO::File::LoadFromFile

#include <antares/benchmarking/DurationCollector.h>
#include <antares/benchmarking/timer.h>
#include <antares/logs/logs.h>
#include <antares/writer/in_memory_writer.h>

namespace Antares::Solver
{

namespace
{
void logErrorAndThrow [[noreturn]] (const std::string& errorMessage)
{
    Antares::logs.error() << errorMessage;
    throw IOError(errorMessage);
}

  template<class ContentT>
  void addToMap(InMemoryWriter::MapType& entries,
                const std::string& entryPath,
                ContentT& content,
                std::mutex& mutex,
                Benchmarking::DurationCollector& duration_collector)
  {
    std::string entryPathSanitized = entryPath;
    std::replace(entryPathSanitized.begin(), entryPathSanitized.end(), '\\', '/');

    Benchmarking::Timer timer_wait;
    std::lock_guard lock(mutex);
    timer_wait.stop();
    duration_collector.addDuration("in_memory_wait", timer_wait.get_duration());

    Benchmarking::Timer timer_insert;
    entries.insert({entryPathSanitized, content});
    timer_insert.stop();
    duration_collector.addDuration("in_memory_insert", timer_insert.get_duration());
}
} // namespace


InMemoryWriter::InMemoryWriter(Benchmarking::DurationCollector& duration_collector) : pDurationCollector(duration_collector) {}

InMemoryWriter::~InMemoryWriter() = default;

void InMemoryWriter::addEntryFromBuffer(const std::string& entryPath, Yuni::Clob& entryContent)
{
    addToMap(pEntries, entryPath, entryContent, pMapMutex, pDurationCollector);
}

void InMemoryWriter::addEntryFromBuffer(const std::string& entryPath, std::string& entryContent)
{
    addToMap(pEntries, entryPath, entryContent, pMapMutex, pDurationCollector);
}

void InMemoryWriter::addEntryFromFile(const std::string& entryPath, const std::string& filePath)
{
    // Shamelessly copy-pasted from zip_writer.cpp
    // TODO refactor
    Yuni::Clob buffer;
    switch (Yuni::IO::File::LoadFromFile(buffer, filePath.c_str()))
    {
        using namespace Yuni::IO;
    case errNone:
        addToMap(pEntries, entryPath, buffer, pMapMutex, pDurationCollector);
        break;
    // Since logErrorAndThrow does not return, we don't need 'break's here
    case errNotFound:
        logErrorAndThrow(filePath + ": file does not exist");
    case errReadFailed:
        logErrorAndThrow("Read failed '" + filePath + "'");
    case errMemoryLimit:
        logErrorAndThrow("Size limit hit for file '" + filePath + "'");
    default:
        logErrorAndThrow("Unhandled error");
    }
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
