
// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/modeler/fileWriter/FileWriter.h"

#include <fstream>

#include <antares/logs/logs.h>
#include "antares/io/outputs/SimulationTableCsvFile.h"
#include "antares/solver/modeler/Modeler.h"

using namespace Antares::IO::Outputs;

namespace Antares::Solver
{
void FileWriter::init(const std::string& simulationId)
{
    outputPath_ = studyPath_ / "output";
    simulationId_ = simulationId;
    logs.info() << "Output folder : " << outputPath_;
    if (!std::filesystem::is_directory(outputPath_)
        && !std::filesystem::create_directory(outputPath_))
    {
        throw Solver::Modeler::ModelerError(
          "Failed to create output directory. Exiting simulation.");
    }
}

const std::filesystem::path& FileWriter::outputPath() const
{
    return outputPath_;
}

void FileWriter::writeSimulationTable(SimulationTableCsv& SimulationTable) const
{
    IO::Outputs::SimulationTableCsvFile writer(outputPath_, simulationId_, parquetFormatRequired_);
    SimulationTable.writeHeaderToBuffer();
    writer.write(SimulationTable);
}

FileWriter::FileWriter(std::filesystem::path path, bool parquetFormatRequired):
    studyPath_(std::move(path)),
    parquetFormatRequired_(parquetFormatRequired)
{
}

} // namespace Antares::Solver
