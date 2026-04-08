// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include "i_table_writer.h"
#include "simulation-table-writers_export.h"

namespace Antares::Writer
{
// gp : here we return a shared ptr. Why not rather a unique ptr ?
SIMULATION_TABLE_WRITERS_EXPORT ITableWriter::Ptr makeTableWriter(bool parquetFormatRequired,
                                                                  std::filesystem::path& filePath);

} // namespace Antares::Writer
