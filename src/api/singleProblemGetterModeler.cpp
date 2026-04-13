// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/api/singleProblemGetterModeler.h"

#include "singleProblemGetterModelerImpl.h"

namespace Antares::Solver
{
SingleProblemGetterModeler::SingleProblemGetterModeler(const std::filesystem::path& modelerPath):
    impl_(std::make_unique<Implementation::SingleProblemGetterModeler>(modelerPath))
{
}

SingleProblemGetterModeler::~SingleProblemGetterModeler() = default;

void SingleProblemGetterModeler::printProblems() const
{
    impl_->printProblems();
}

} // namespace Antares::Solver
