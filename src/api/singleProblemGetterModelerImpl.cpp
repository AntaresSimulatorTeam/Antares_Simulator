// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "singleProblemGetterModelerImpl.h"

#include "antares/logs/logs.h"
#include "antares/solver/modeler/fileWriter/FileWriter.h"
#include "antares/solver/modeler/loadFiles/Fileloader.h"

namespace Antares::Solver::Implementation
{

SingleProblemGetterModeler::SingleProblemGetterModeler(const std::filesystem::path& modelerPath)
{
    auto loader = std::make_unique<Antares::Solver::LoadFiles::FileLoader>(modelerPath);
    auto writer = std::make_unique<Antares::Solver::FileWriter>(modelerPath);
    modeler_ = std::make_unique<Modeler>(*loader, *writer);
    modeler_->buildProblems();
}

void SingleProblemGetterModeler::printProblems() const
{
    const auto& master = modeler_->masterProblem();
    const auto& subproblems = modeler_->subproblems();

    auto printProblem = [](const auto& problem, const char* name)
    {
        if (problem)
        {
            logs.info() << name << ": " << problem->variableCount() << " variables, "
                        << problem->constraintCount() << " constraints";
        }
    };

    printProblem(master, "Master problem");
    for (size_t i = 0; i < subproblems.size(); ++i)
    {
        printProblem(subproblems[i], ("Subproblem " + std::to_string(i + 1)).c_str());
    }
}

} // namespace Antares::Solver::Implementation
