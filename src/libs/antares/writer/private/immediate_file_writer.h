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

#include <filesystem>
#include <string>

#include <yuni/core/string.h>

#include "antares/writer/i_writer.h"

namespace Antares::Solver
{
class ImmediateFileResultWriter: public IResultWriter
{
public:
    ImmediateFileResultWriter(const std::filesystem::path& folderOutput);
    virtual ~ImmediateFileResultWriter();
    // Write to file immediately, creating directories if needed
    void addEntryFromBuffer(const std::string& entryPath, Yuni::Clob& entryContent) override;
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
