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

    output_file_ = outputPath_ / "simulation_table";

    writer_ = makeTableWriter(fmt_, output_file_);
    logs.info() << "Simulation table is written in: " << output_file_.string();
}

const std::filesystem::path& FileWriter::outputPath() const
{
    return outputPath_;
}

void FileWriter::writeSimulationTable(SimulationTable& SimulationTable) const
{
    writer_->writeTable(SimulationTable);
}

FileWriter::FileWriter(const std::filesystem::path& studyPath, Writer::TableFormat fmt):
    studyPath_(studyPath),
    fmt_(fmt)
{
}

} // namespace Antares::Solver
