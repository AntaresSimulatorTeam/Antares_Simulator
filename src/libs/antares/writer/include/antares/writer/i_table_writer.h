// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <filesystem>
#include <memory>
#include <string>
#include <vector>

namespace Antares::Writer
{

class ITableWriter
{
public:
    using Ptr = std::shared_ptr<ITableWriter>;
    virtual ~ITableWriter() = default;

    // Header + rows of string cells
    virtual void writeTable(const std::filesystem::path& filePath,
                            const std::vector<std::string>& header,
                            const std::vector<std::vector<std::string>>& rows)
      = 0;
};

} // namespace Antares::Writer
