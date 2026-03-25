
// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/modeler/fileWriter/FileWriter.h"

#include <fstream>

#include <antares/logs/logs.h>
#include <antares/optimisation/linear-problem-mpsolver-impl/linearProblem.h>
#include <antares/optimisation/linear-problem-mpsolver-impl/mipSolution.h>
#include <antares/solver/optim-model-filler/Dimensions.h>
#include <antares/study/system-model/component.h>
#include "antares/io/outputs/SimulationTableCsvFile.h"
#include "antares/io/outputs/SimulationTableGenerator.h"
#include "antares/solver/modeler/Modeler.h"

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

void FileWriter::writeSimulationTable(
  const Optimisation::LinearProblemApi::ILinearProblem& linearProblem,
  const Optimisation::LinearProblemApi::IMipSolution& solution,
  const ModelerData& modelerData,
  const Optimisation::OptimEntityContainer& variableContainer,
  const Optimisation::LinearProblemApi::FillContext& fillContext) const
{
    // gp : We are in file writer, and though we build a simulation table AND write it on disk.
    // gp : Building the simulation table is not the responsibility of the writer, it should have
    // gp : been done earlier, and the table should be passed as argument.
    // gp : Doing so, we would dramatically reduce the number of arguments of this function.
    IO::Outputs::SimulationTableCsvFile simulationTable(outputPath_,
                                                        simulationId_,
                                                        parquetFormatRequired_);
    IO::Outputs::FillSimulationTable(simulationTable,
                                     linearProblem,
                                     solution.getObjectiveValue(),
                                     modelerData,
                                     variableContainer,
                                     fillContext,
                                     0,
                                     IO::Outputs::TimeConversionMode::SingleBlock);

    // gp : Simulation table and writing it should be decoupled and separate classes.
    // gp : We should have a simulation table writer that takes a simulation table at constuction.
    simulationTable.writeHeaderToBuffer();
    simulationTable.write();
}

FileWriter::FileWriter(std::filesystem::path path, bool parquetFormatRequired):
    studyPath_(std::move(path)),
    parquetFormatRequired_(parquetFormatRequired)
{
}

} // namespace Antares::Solver
