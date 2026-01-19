
// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "FileWriter.h"

#include <fstream>

#include <antares/logs/logs.h>
#include <antares/optimisation/linear-problem-mpsolver-impl/linearProblem.h>
#include <antares/optimisation/linear-problem-mpsolver-impl/mipSolution.h>
#include <antares/solver/optim-model-filler/Dimensions.h>
#include <antares/study/system-model/component.h>
#include "antares/io/outputs/SimulationTableCsvFile.h"
#include "antares/io/outputs/SimulationTableGenerator.h"

#include "modeler/include/antares/solver/modeler/Modeler.h"

namespace Antares::Modeler
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
  const Data& modelerData,
  const Optimisation::OptimEntityContainer& variableContainer,
  const Optimisation::LinearProblemApi::FillContext& fillContext) const
{
    SimulationTableCsvFile simulationTable(outputPath_, simulationId_);
    IO::FillSimulationTable(simulationTable,
                            linearProblem,
                            solution.getObjectiveValue(),
                            modelerData,
                            variableContainer,
                            fillContext,
                            0,
                            IO::TimeConversionMode::SingleBlock);
    simulationTable.writeHeader();
    simulationTable.write();
}

FileWriter::FileWriter(std::filesystem::path path):
    studyPath_(std::move(path))
{
}

} // namespace Antares::Modeler
