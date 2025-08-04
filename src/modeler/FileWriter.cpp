
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

#include "modeler/include/antares/solver/modeler/Modeler.h"

namespace Antares::Modeler
{
void FileWriter::init(bool setOutput)
{
    output = setOutput;
    outputPath_ = studyPath_ / "output";
    if (output)
    {
        logs.info() << "Output folder : " << outputPath_;
        if (!std::filesystem::is_directory(outputPath_)
            && !std::filesystem::create_directory(outputPath_))
        {
            throw Antares::Solver::Modeler::Error(
              "Failed to create output directory. Exiting simulation.");
        }
    }
}

void FileWriter::writeSolution(const Optimisation::LinearProblemApi::IMipSolution& solution)
{
    if (output)
    {
        logs.info() << "Writing objective & variable values...";
        std::ofstream sol_out(outputPath_ / "solution.csv");
        sol_out << std::setprecision(15) << "objective " << solution.getObjectiveValue()
                << std::endl;
        for (const auto& [name, value]: solution.getOptimalValues())
        {
            sol_out << name << " " << value << std::endl;
        }
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
