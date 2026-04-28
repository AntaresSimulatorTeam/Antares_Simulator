// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <filesystem>
#include <memory>
#include <stdexcept>
#include <string>

namespace Antares::Solver
{

class IResultWriter
{
public:
    using Ptr = std::shared_ptr<IResultWriter>;

    virtual ~IResultWriter() = default;

    virtual void addEntryFromBuffer(const std::filesystem::path& entryPath,
                                    std::string& entryContent)
      = 0;
    virtual void addEntryFromFile(const std::filesystem::path& entryPath,
                                  const std::filesystem::path& filePath)
      = 0;

    /*!
     * Waits for completion of every write operation previously appended.
     * A runtime error may be raised if any of those fails.
     */
    virtual void flush() = 0;
    virtual bool needsTheJobQueue() const = 0;
    virtual void finalize(bool verbose) = 0;
};
} // namespace Antares::Solver
