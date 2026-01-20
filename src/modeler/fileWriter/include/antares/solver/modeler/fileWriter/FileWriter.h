// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once
#include <filesystem>

#include "antares/solver/modeler/IWriter.h"

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

namespace Antares::Solver
{
class FileWriter: public IWriter
{
public:
    void init(const std::string& simulationId) override;

    void writeSimulationTable(
      const Optimisation::LinearProblemApi::ILinearProblem& linearProblem,
      const Optimisation::LinearProblemApi::IMipSolution& solution,
      const ModelerData& modelerData,
      const Optimisation::OptimEntityContainer& variableContainer,
      const Optimisation::LinearProblemApi::FillContext& fillContext) const override;
    explicit FileWriter(std::filesystem::path path);

    const std::filesystem::path& outputPath() const;

private:
    const std::filesystem::path studyPath_;
    std::filesystem::path outputPath_;
    std::string simulationId_;
};
} // namespace Antares::Solver
