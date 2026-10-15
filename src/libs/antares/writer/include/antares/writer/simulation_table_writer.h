// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <filesystem>

#include "table_format.h"

namespace Antares::IO::Outputs
{
class SimulationTable;
} // namespace Antares::IO::Outputs

namespace Antares::Writer
{

class SimulationTableWriter final
{
public:
    SimulationTableWriter(const std::filesystem::path& filePath, TableFormat tableFormat);
    void writeTable(const IO::Outputs::SimulationTable& simuTable) const;

    [[nodiscard]] const std::filesystem::path& outputFile() const
    {
        return output_file_;
    }

private:
    void writeParquet(const std::filesystem::path& file_path,
                      const IO::Outputs::SimulationTable& simuTable) const;
    void writeCsv(const std::filesystem::path& file_path,
                  const IO::Outputs::SimulationTable& simuTable) const;

    std::filesystem::path output_file_;
    TableFormat table_format_;
};

} // namespace Antares::Writer
