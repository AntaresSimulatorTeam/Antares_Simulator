// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/api/modelerProblems.h"

#include "modelerProblemsImpl.h"

namespace Antares::Solver
{
ModelerProblems::ModelerProblems(const std::filesystem::path& modelerPath):
    impl_(std::make_unique<Implementation::ModelerProblems>(modelerPath))
{
}

ModelerProblems::~ModelerProblems() = default;

void ModelerProblems::logSize() const
{
    impl_->logSize();
}

} // namespace Antares::Solver
