// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "include/antares/writer/LegacySimulationTablesWriter.h"

#include <antares/exception/RuntimeError.hpp>

#include "include/antares/writer/table_writer_factory.h"

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
                                      const unsigned optim_nb)
{
    // File name without extension
    std::string filename = "simulation-table-" + std::to_string(year) + "-optim-nb-"
                           + std::to_string(optim_nb);
    return parentFolder / filename;
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
    auto filepath = makeSimuTableFilePath(folder_, year_, optim_number);
    ITableWriter::Ptr writer = makeTableWriter(tableFormat_, filepath);
    writer->writeTable(*table);
}

} // namespace Antares::Writer
