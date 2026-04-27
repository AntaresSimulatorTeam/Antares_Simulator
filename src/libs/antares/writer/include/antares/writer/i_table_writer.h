// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <filesystem>
#include <memory>
#include <string>
#include <vector>

#include "antares/io/outputs/SimulationTable.h"

namespace Antares::Writer
{

class ITableWriter
{
public:
    using Ptr = std::shared_ptr<ITableWriter>;

    explicit ITableWriter(std::filesystem::path& filePath);
    virtual ~ITableWriter() = default;
    virtual void writeTable(const IO::Outputs::SimulationTable& simuTable) const = 0;

protected:
    std::filesystem::path& output_file_;
};

} // namespace Antares::Writer
