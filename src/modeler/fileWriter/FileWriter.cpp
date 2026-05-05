// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/modeler/fileWriter/FileWriter.h"

#include <antares/logs/logs.h>
#include <antares/writer/table_writer_factory.h>
#include "antares/exception/InvalidArgumentError.hpp"
#include "antares/exception/RuntimeError.hpp"
#include "antares/io/outputs/SimulationTable.h"
#include "antares/solver/modeler/Modeler.h"
#include "antares/utils/utils.h"

using namespace Antares::IO::Outputs;
using namespace Antares::Writer;
namespace fs = std::filesystem;

namespace Antares::Solver
{
void FileWriter::init(const std::string& simulationId)
{
    simulationId_ = simulationId;

    if (!fs::is_directory(outputPath_) && !fs::create_directory(outputPath_))
    {
        throw Modeler::ModelerError("Failed to create output directory. Exiting simulation.");
    }

    const auto simulation_id = std::string(simulationId.empty() ? "" : "--" + simulationId);
    output_file_ = outputPath_ / ("simulation_table" + simulation_id);

    // TODO : Here we pass the simulation table output path to a more specific writer (csv /
    // TODO : parquet), which appends the right extension. So one part of absolute path
    // TODO : is done here, and the other one in another writer.
    // TODO : Thinking about this, class FileWriter is useless.
    // TODO : Removing it, making the specific writer have resonsibility to create this path
    // TODO : and directly using the specific writer in Modeler would solve the problem
    // TODO : and simplify the code.
    writer_ = makeTableWriter(parquetFormatRequired_, output_file_);
    Antares::logs.info() << "Simulation table is written in: " << output_file_.string();
}

const std::filesystem::path& FileWriter::outputPath() const
{
    return outputPath_;
}

void FileWriter::writeSimulationTable(SimulationTable& simulationTable) const
{
    writer_->writeTable(simulationTable);
}

FileWriter::FileWriter(const std::filesystem::path& studyPath, bool parquetFormatRequired):
    parquetFormatRequired_(parquetFormatRequired)
{
    if (!fs::exists(studyPath))
    {
        throw std::runtime_error("Could not find output Folder: " + studyPath.string());
    }
    outputPath_ = std::move(studyPath) / "output";
    logs.info() << "Output folder : " << outputPath_;
}

} // namespace Antares::Solver
