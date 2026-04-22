#include "include/antares/writer/LegacySimulationTablesWriter.h"

#include "include/antares/writer/table_writer_factory.h"

namespace fs = std::filesystem;
using namespace Antares::IO::Outputs;

namespace Antares::Writer
{
static void check_is_existing_folder(const fs::path& folder)
{
    if (!fs::is_directory(folder))
    {
        std::string msg = "Output folder " + folder.string() + "is does not exist.";
        throw std::invalid_argument(msg);
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

LegacySimulationTablesWriter::LegacySimulationTablesWriter(const fs::path folder,
                                                           const unsigned year,
                                                           bool parquetFormat):
    year_(year),
    parquetFormat_(parquetFormat),
    folder_(std::move(folder))
{
    check_is_existing_folder(folder);
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
    ITableWriter::Ptr writer = makeTableWriter(parquetFormat_, filepath);
    writer->writeTable(*table);
}

} // namespace Antares::Writer
