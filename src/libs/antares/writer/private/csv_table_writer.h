// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include "antares/writer/i_table_writer.h"

#include "simulation-table-writers_export.h"

namespace Antares::Writer
{

class SIMULATION_TABLE_WRITERS_EXPORT CsvTableWriter final: public ITableWriter
{
public:
    explicit CsvTableWriter(const std::filesystem::path& filePath);
    void writeTable(const IO::Outputs::SimulationTable& simuTable) const override;
};

} // namespace Antares::Writer
