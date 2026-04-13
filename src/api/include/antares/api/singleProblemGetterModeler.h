// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <filesystem>
#include <memory>
#include <string>
#include <vector>

#include "antares/solver/modeler/Modeler.h"

namespace Antares::Solver::Implementation
{
class SingleProblemGetterModeler;
}

namespace Antares::Solver
{
class SingleProblemGetterModeler final
{
public:
    explicit SingleProblemGetterModeler(const std::filesystem::path& modelerPath);
    ~SingleProblemGetterModeler();

    std::vector<std::string> getProblemIds() const;
    std::unique_ptr<Optimisation::LinearProblemApi::ILinearProblem> getProblem(
      const std::string& problemId);

    void printProblems() const;
    Solver::ProblemEntity getMasterProblem() const;

private:
    std::unique_ptr<Implementation::SingleProblemGetterModeler> impl_;
};
} // namespace Antares::Solver
