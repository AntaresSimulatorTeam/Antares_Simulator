// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <filesystem>
#include <string>

#include "antares/io/outputs/OptimisationsSimulationTable.h"
#include "antares/io/outputs/SimulationTable.h"

#include "simulation_table_writer.h"
#include "table_format.h"

namespace Antares::Writer
{
class LegacySimulationTablesWriter
{
public:
    LegacySimulationTablesWriter(const std::filesystem::path& folder,
                                 const unsigned year,
                                 TableFormat tableFormat = TableFormat::CSV);
    void write(IO::Outputs::OptimisationsSimulationTable& tables) const;

private:
    void writeForStage(const IO::Outputs::SimulationTable& table, const std::string& stage) const;
    unsigned year_ = 0;
    TableFormat tableFormat_;
    const std::filesystem::path folder_;
};
} // namespace Antares::Writer
