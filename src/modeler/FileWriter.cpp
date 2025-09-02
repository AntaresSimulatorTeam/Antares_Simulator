
// Copyright 2007-2025, RTE (https://www.rte-france.com)
// See AUTHORS.txt
// SPDX-License-Identifier: MPL-2.0
// This file is part of Antares-Simulator,
// Adequacy and Performance assessment for interconnected energy networks.
//
// Antares_Simulator is free software: you can redistribute it and/or modify
// it under the terms of the Mozilla Public Licence 2.0 as published by
// the Mozilla Foundation, either version 2 of the License, or
// (at your option) any later version.
//
// Antares_Simulator is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// Mozilla Public Licence 2.0 for more details.
//
// You should have received a copy of the Mozilla Public Licence 2.0
// along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.

#include "FileWriter.h"

#include <fstream>

#include <antares/logs/logs.h>
#include <antares/optimisation/linear-problem-mpsolver-impl/linearProblem.h>
#include <antares/optimisation/linear-problem-mpsolver-impl/mipSolution.h>
#include <antares/solver/optim-model-filler/VariableDictionary.h>
#include <antares/study/system-model/component.h>
#include "antares/io/outputs/SimulationTableCsvFile.h"
#include "antares/io/outputs/SimulationTableGenerator.h"

#include "modeler/include/antares/solver/modeler/Modeler.h"

namespace Antares::Modeler
{
void FileWriter::init(bool setOutput, const std::string& simulationId)
{
    output = setOutput;
    outputPath_ = studyPath_ / "output";
    simulationId_ = simulationId;
    if (output)
    {
        logs.info() << "Output folder : " << outputPath_;
        if (!std::filesystem::is_directory(outputPath_)
            && !std::filesystem::create_directory(outputPath_))
        {
            throw Solver::Modeler::ModelerError(
              "Failed to create output directory. Exiting simulation.");
        }
    }
}

void FileWriter::writeSimulationTable(
  const Optimisation::LinearProblemApi::ILinearProblem& linearProblem,
  const Optimisation::LinearProblemApi::IMipSolution& solution,
  const std::unordered_map<std::string, ModelerStudy::SystemModel::Component>& components,
  const Optimization::VariableDictionary& variableDictionary,
  const Optimisation::LinearProblemApi::FillContext& fillContext) const
{
    if (output)
    {
        SimulationTableCsvFile simulationTable(outputPath_, simulationId_);
        FillSimulationTable(simulationTable,
                            linearProblem,
                            solution.getObjectiveValue(),
                            components,
                            fillContext,
                            1,
                            TimeConversionMode::SingleBlock);
        simulationTable.writeHeader();
        simulationTable.write();
    }
}

FileWriter::FileWriter(std::filesystem::path path):
    studyPath_(std::move(path))
{
}

void FileWriter::writeProblem(
  const Optimisation::LinearProblemMpsolverImpl::OrtoolsLinearProblem& problem)
{
    if (output)
    {
        logs.info() << "Writing problem.lp...";
        const auto lp_path = outputPath_ / "problem.lp";
        problem.WriteLP(lp_path.string());
    }
}
} // namespace Antares::Modeler
