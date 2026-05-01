// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include "antares/writer/i_table_writer.h"

namespace Antares::Writer
{

class CsvTableWriter final: public ITableWriter
{
public:
    explicit CsvTableWriter(std::filesystem::path& filePath);
    void writeTable(const IO::Outputs::SimulationTable& simuTable) const override;
};

} // namespace Antares::Writer
