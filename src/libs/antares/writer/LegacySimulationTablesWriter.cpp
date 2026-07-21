// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "include/antares/writer/LegacySimulationTablesWriter.h"

#include <antares/exception/RuntimeError.hpp>

#include "antares/writer/parquet_table_writer.h"

namespace fs = std::filesystem;
using namespace Antares::IO::Outputs;
using namespace Antares::Error;

namespace Antares::Writer
{
static void check_is_existing_folder(const fs::path& folder)
{
    if (!fs::is_directory(folder))
    {
        std::string msg = "Output folder " + folder.string() + " does not exist.";
        throw RuntimeError(msg);
    }
}

static fs::path makeSimuTableFilePath(const fs::path& parentFolder,
                                      const unsigned year,
                                      const unsigned optim_nb,
                                      TableFormat tableFormat)
{
    // File name without extension
    std::string filename = "simulation-table-" + std::to_string(year) + "-optim-nb-"
                           + std::to_string(optim_nb);
    auto filepath = parentFolder / filename;

    // Add appropriate extension based on format
    if (tableFormat == TableFormat::CSV)
    {
        filepath.replace_extension(".csv");
    }
    else
    {
        filepath.replace_extension(".parquet");
    }
    return filepath;
}

LegacySimulationTablesWriter::LegacySimulationTablesWriter(const fs::path& folder,
                                                           const unsigned year,
                                                           TableFormat tableFormat):
    year_(year),
    tableFormat_(tableFormat),
    folder_(folder)
{
    check_is_existing_folder(folder_);
}

void LegacySimulationTablesWriter::write(OptimisationsSimulationTable& tables)
{
    writeForOptim(tables.firstOptimSimulationTable(), 1);
    writeForOptim(tables.secondOptimSimulationTable(), 2);
}

void LegacySimulationTablesWriter::writeForOptim(const SimulationTable* table,
                                                 unsigned optim_number)
{
    auto filepath = makeSimuTableFilePath(folder_, year_, optim_number, tableFormat_);
    auto writer = std::make_unique<ParquetTableWriter>(filepath, tableFormat_);
    writer->writeTable(*table);
}

} // namespace Antares::Writer
