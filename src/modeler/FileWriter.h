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

#pragma once
#include <filesystem>

#include "modeler/include/antares/solver/modeler/IWriter.h"

namespace Antares::Optimisation::LinearProblemApi
{
class ILinearProblem;
class ILinearProblemData;
class FillContext;
} // namespace Antares::Optimisation::LinearProblemApi

namespace Antares::Optimization
{
class VariableDictionary;
}

namespace Antares::ModelerStudy::SystemModel
{
class Component;
}

namespace Antares::Modeler
{
class FileWriter: public Solver::IWriter
{
public:
    void init(const std::string& simulationId) override;

    void writeSimulationTable(
      const Optimisation::LinearProblemApi::ILinearProblem& linearProblem,
      const Optimisation::LinearProblemApi::IMipSolution& solution,
      const Data& modelerData,
      const Optimisation::OptimEntityContainer& variableContainer,
      const Optimisation::LinearProblemApi::FillContext& fillContext) const override;
    explicit FileWriter(std::filesystem::path path);

    const std::filesystem::path& outputPath() const;

private:
    const std::filesystem::path studyPath_;
    std::filesystem::path outputPath_;
    std::string simulationId_;
};
} // namespace Antares::Modeler
