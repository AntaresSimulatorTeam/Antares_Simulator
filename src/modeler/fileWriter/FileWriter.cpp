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
    if (time.empty())
    {
        throw Modeler::ModelerError("Time identifier cannot be empty. Exiting simulation.");
    }

    outputPath_ = studyPath_ / "output" / time;

    // avoid overwriting existing output by adding a suffix (-2, -3, etc.)
    if (!Utils::generatePathWithSuffix(outputPath_))
    {
        throw Modeler::ModelerError("Output folder already exists: " + outputPath_.string());
    }

    logs.info() << "Output folder : " << outputPath_;
    if (!fs::is_directory(outputPath_) && !fs::create_directories(outputPath_))
    {
        throw Modeler::ModelerError("Failed to create output directory. Exiting simulation.");
    }

    output_file_ = outputPath_ / "simulation-table";

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
    studyPath_(studyPath),
    parquetFormatRequired_(parquetFormatRequired)
{
}

} // namespace Antares::Solver
