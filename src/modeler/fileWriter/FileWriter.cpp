// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/modeler/fileWriter/FileWriter.h"

#include <antares/logs/logs.h>
#include <antares/optimisation/linear-problem-mpsolver-impl/linearProblem.h>
#include <antares/optimisation/linear-problem-mpsolver-impl/mipSolution.h>
#include <antares/solver/optim-model-filler/Dimensions.h>
#include <antares/study/system-model/component.h>
#include "antares/io/outputs/SimulationTableCsvFile.h"
#include "antares/io/outputs/SimulationTableGenerator.h"
#include "antares/solver/modeler/Modeler.h"
#include "antares/utils/utils.h"

namespace Antares::Solver
{
void FileWriter::init(const std::string& time)
{
    outputPath_ = studyPath_ / "output" / time;

    // avoid overwriting existing output by adding a suffix (-2, -3, etc.)
    if (!Utils::generatePathWithSuffix(outputPath_))
    {
        throw Modeler::ModelerError("Output folder already exists: " + outputPath_.string());
    }

    logs.info() << "Output folder : " << outputPath_;
    if (!std::filesystem::is_directory(outputPath_)
        && !std::filesystem::create_directories(outputPath_))
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
  const Optimisation::LinearProblemApi::FillContext& fillContext,
  const std::string& simulationTableName) const
{
    IO::Outputs::SimulationTableCsvFile simulationTable(outputPath_, simulationTableName);
    IO::Outputs::FillSimulationTable(simulationTable,
                                     linearProblem,
                                     solution.getObjectiveValue(),
                                     modelerData,
                                     variableContainer,
                                     fillContext,
                                     0,
                                     IO::Outputs::TimeConversionMode::SingleBlock);
    simulationTable.writeHeader();
    simulationTable.write();
}

FileWriter::FileWriter(std::filesystem::path path):
    studyPath_(std::move(path))
{
}

} // namespace Antares::Solver
