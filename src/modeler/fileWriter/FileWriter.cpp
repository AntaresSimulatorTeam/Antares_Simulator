// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/modeler/fileWriter/FileWriter.h"

#include <antares/logs/logs.h>
#include <antares/writer/table_writer_factory.h>
#include "antares/io/outputs/SimulationTable.h"
#include "antares/solver/modeler/Modeler.h"
#include "antares/utils/utils.h"

using namespace Antares::IO::Outputs;
using namespace Antares::Writer;
namespace fs = std::filesystem;

namespace Antares::Solver
{
void FileWriter::init(const std::string& time)
{
}

const std::filesystem::path& FileWriter::outputPath() const
{
    return outputPath_;
}

void FileWriter::writeSimulationTable(SimulationTable& simulationTable) const
{
    writer_->writeTable(simulationTable);
}

FileWriter::FileWriter(const std::filesystem::path& studyPath, TableFormat tableFormat):
    studyPath_(studyPath),
    tableFormat_(tableFormat)
{
}

} // namespace Antares::Solver
