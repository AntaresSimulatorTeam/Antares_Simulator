// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include "include/antares/writer/i_table_writer.h"

namespace Antares::Writer
{

class CsvTableWriter final: public ITableWriter
{
public:
    void writeTable(const std::filesystem::path& filePath,
                    const std::vector<std::string>& header,
                    const std::vector<std::vector<std::string>>& rows) override;
};

} // namespace Antares::Writer
