// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <filesystem>
#include <memory>

#include <antares/optimisation/linear-problem-api/linearProblem.h>

namespace Antares::Solver::Implementation
{
class ModelerProblems;
}

namespace Antares::Solver
{
class ModelerProblems final
{
public:
    explicit ModelerProblems(const std::filesystem::path& modelerPath);
    ~ModelerProblems();

    void logSize() const;

private:
    std::unique_ptr<Implementation::ModelerProblems> impl_;
};
} // namespace Antares::Solver
