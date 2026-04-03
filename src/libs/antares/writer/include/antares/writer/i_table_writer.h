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
    virtual ~ITableWriter() = default;

    virtual void writeTable(const std::filesystem::path& filePath,
                            const IO::Outputs::SimulationTable& simuTable)
      = 0;
};

} // namespace Antares::Writer
