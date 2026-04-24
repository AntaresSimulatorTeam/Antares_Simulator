
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

constexpr unsigned maxFolderSameTime = 2000;

namespace Antares::Solver
{
void FileWriter::init(const std::string& time)
{
    outputPath_ = studyPath_ / "output" / time;

    // avoid overwriting existing output by adding a suffix
    if (std::filesystem::exists(outputPath_))
    {
        std::string candidate;
        uint index = 1;
        do
        {
            ++index;
            candidate = outputPath_.string() + '-' + std::to_string(index);
        } while (std::filesystem::exists(candidate) && index < maxFolderSameTime);

        outputPath_ += '-' + std::to_string(index);
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
