// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "include/antares/writer/table_writer_factory.h"

#include <memory>

#include "csv_table_writer.h"
#include "parquet_table_writer.h"

namespace fs = std::filesystem;

namespace Antares::Writer
{

ITableWriter::Ptr makeTableWriter(TableFormat tableFormat, fs::path& filePath)
{
    if (tableFormat == TableFormat::Parquet)
    {
        filePath.replace_extension(".parquet");
        return std::make_shared<ParquetTableWriter>(filePath);
    }

    filePath.replace_extension(".csv");
    return std::make_shared<CsvTableWriter>(filePath);
}

} // namespace Antares::Writer
