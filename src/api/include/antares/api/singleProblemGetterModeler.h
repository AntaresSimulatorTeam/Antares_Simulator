// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <filesystem>
#include <memory>

#include <antares/optimisation/linear-problem-api/linearProblem.h>

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

    void printProblems() const;

private:
    std::unique_ptr<Implementation::SingleProblemGetterModeler> impl_;
};
} // namespace Antares::Solver
