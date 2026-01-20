// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <filesystem>
#include <string>

#include <yuni/core/string.h>

#include "antares/writer/i_writer.h"

namespace Antares::Solver
{
class ImmediateFileResultWriter final: public IResultWriter
{
public:
    ImmediateFileResultWriter(const std::filesystem::path& folderOutput);
    virtual ~ImmediateFileResultWriter();
    // Write to file immediately, creating directories if needed
    void addEntryFromBuffer(const std::filesystem::path& entryPath,
                            std::string& entryContent) override;
    void addEntryFromFile(const std::filesystem::path& entryPath,
                          const std::filesystem::path& filePath) override;
    void flush() override;
    bool needsTheJobQueue() const override;
    void finalize(bool verbose) override;

private:
    const std::filesystem::path pOutputFolder;
};
} // namespace Antares::Solver
