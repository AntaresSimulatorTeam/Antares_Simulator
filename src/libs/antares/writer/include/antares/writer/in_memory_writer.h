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
#pragma once

#include <map>
#include <mutex>
#include <string>

#include <yuni/core/string.h>

#include <antares/benchmarking/DurationCollector.h>
#include "antares/writer/i_writer.h"

namespace Antares::Solver
{
class InMemoryWriter: public IResultWriter
{
public:
    using MapType = std::map<std::string, std::string, std::less<>>;
    explicit InMemoryWriter(Benchmarking::DurationCollector& duration_collector);
    virtual ~InMemoryWriter();
    void addEntryFromBuffer(const std::string& entryPath, Yuni::Clob& entryContent) override;
    void addEntryFromBuffer(const std::filesystem::path& entryPath,
                            std::string& entryContent) override;
    void addEntryFromFile(const std::filesystem::path& entryPath,
                          const std::filesystem::path& filePath) override;
    void flush() override;
    bool needsTheJobQueue() const override;
    void finalize(bool verbose) override;
    const MapType& getMap() const;

private:
    std::mutex pMapMutex;
    // minizip-ng requires a void* as a zip handle.
    MapType pEntries;
    // State, to allow/prevent new jobs being added to the queue
    Benchmarking::DurationCollector& pDurationCollector;
};
} // namespace Antares::Solver
