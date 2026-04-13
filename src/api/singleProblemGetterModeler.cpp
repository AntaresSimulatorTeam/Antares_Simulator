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

std::vector<std::string> SingleProblemGetterModeler::getProblemIds() const
{
    return impl_->getProblemIds();
}

std::unique_ptr<Optimisation::LinearProblemApi::ILinearProblem>
SingleProblemGetterModeler::getProblem(const std::string& problemId)
{
    return impl_->getProblem(problemId);
}

Solver::ProblemEntity SingleProblemGetterModeler::getMasterProblem() const
{
    return impl_->getMasterProblem();
}

void SingleProblemGetterModeler::printProblems() const
{
    impl_->printProblems();
}

} // namespace Antares::Solver
