// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <map>
#include <mutex>
#include <string>

#include <yuni/core/string.h>

#include <antares/benchmarking/DurationCollector.h>
#include "antares/writer/i_writer.h"

namespace Antares::Solver
{
class InMemoryWriter final: public IResultWriter
{
public:
    using MapType = std::map<std::string, std::string, std::less<>>;
    explicit InMemoryWriter(Benchmarking::DurationCollector& duration_collector);
    virtual ~InMemoryWriter();
    void addEntryFromBuffer(const std::filesystem::path& entryPath,
                            std::string& entryContent) override;
    void addEntryFromFile(const std::filesystem::path& entryPath,
                          const std::filesystem::path& filePath) override;
    void flush() override;
    bool needsTheJobQueue() const override;
    void finalize(bool verbose) override;
    const MapType& getMap() const;

private:
    // prevent concurrent writes
    std::mutex pMapMutex;
    // storage
    MapType pEntries;
    Benchmarking::DurationCollector& pDurationCollector;
};
} // namespace Antares::Solver
