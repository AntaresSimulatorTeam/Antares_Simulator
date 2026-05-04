// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <filesystem>

#include "simulation-table-writers_export.h"
#include "antares/io/outputs/OptimisationsSimulationTable.h"
#include "antares/io/outputs/SimulationTable.h"
#include "i_table_writer.h"

namespace Antares::Writer
{
class SIMULATION_TABLE_WRITERS_EXPORT LegacySimulationTablesWriter
{
public:
    LegacySimulationTablesWriter(const std::filesystem::path folder,
                                 const unsigned year,
                                 bool parquetFormat = false);
    void write(OptimisationsSimulationTable& tables);

private:
    void writeForOptim(const IO::Outputs::SimulationTable* table, unsigned optim_number);
    unsigned year_ = 0;
    bool parquetFormat_ = false;
    const std::filesystem::path folder_;
};
} // namespace Antares::Writer
