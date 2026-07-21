// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <memory>

#include "parquet_table_writer.h"
#include "simulation-table-writers_export.h"
#include "table_format.h"

namespace Antares::Writer
{
SIMULATION_TABLE_WRITERS_EXPORT std::unique_ptr<ParquetTableWriter> makeTableWriter(
  TableFormat tableFormat, std::filesystem::path& filePath);
} // namespace Antares::Writer
