// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "include/antares/writer/table_writer_factory.h"

#include "antares/writer/parquet_table_writer.h"

namespace fs = std::filesystem;

namespace Antares::Writer
{

std::unique_ptr<ParquetTableWriter> makeTableWriter(TableFormat tableFormat, fs::path& filePath)
{
    if (tableFormat == TableFormat::CSV)
    {
        filePath.replace_extension(".csv");
    }
    else
    {
        filePath.replace_extension(".parquet");
    }
    return std::make_unique<ParquetTableWriter>(filePath, tableFormat);
}

} // namespace Antares::Writer
