// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "include/antares/writer/i_table_writer.h"

#include <utility>

namespace Antares::Writer
{

static void ensureParentDir(const std::filesystem::path& file)
{
    auto p = file.parent_path();
    if (!p.empty())
    {
        std::error_code ec;
        std::filesystem::create_directories(p, ec);
        // Best effort; ignore error as writer will fail on open if directory truly invalid
    }
}

ITableWriter::ITableWriter(const std::filesystem::path& filePath):
    output_file_(filePath)
{
    ensureParentDir(output_file_);
}

} // namespace Antares::Writer
