// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "singleProblemGetterModelerImpl.h"

#include <iostream>
#include <stdexcept>

#include "antares/solver/modeler/fileWriter/FileWriter.h"
#include "antares/solver/modeler/loadFiles/Fileloader.h"
#include "antares/writer/i_writer.h"

namespace Antares::Solver::Implementation
{

SingleProblemGetterModeler::SingleProblemGetterModeler(const std::filesystem::path& modelerPath):
    resultWriter_(std::make_shared<NullResultWriter>())
{
    auto loader = std::make_unique<Antares::Solver::LoadFiles::FileLoader>(modelerPath);
    auto writer = std::make_unique<Antares::Solver::FileWriter>(modelerPath);
    modeler_ = std::make_unique<Modeler>(*loader, *writer);
    modeler_->run();
}

std::vector<std::string> SingleProblemGetterModeler::getProblemIds() const
{
    return {"1-1"};
}

std::unique_ptr<Optimisation::LinearProblemApi::ILinearProblem>
SingleProblemGetterModeler::getProblem(const std::string& problemId)
{
    if (problemId != "1-1")
    {
        throw std::invalid_argument("Invalid problem id: " + problemId
                                    + ". Only '1-1' is supported.");
    }
    const auto& subproblemsConst = modeler_->subproblems();
    if (subproblemsConst.empty())
    {
        throw std::runtime_error("No subproblems available");
    }
    auto& subproblems = const_cast<
      std::vector<std::unique_ptr<Optimisation::LinearProblemApi::ILinearProblem>>&>(
      subproblemsConst);
    auto problem = std::move(subproblems[0]);
    subproblems[0] = nullptr;
    return problem;
}

Solver::ProblemEntity SingleProblemGetterModeler::getMasterProblem() const
{
    const auto& masterConst = modeler_->masterProblem();
    if (!masterConst)
    {
        return {nullptr, nullptr};
    }
    auto& master = const_cast<std::unique_ptr<Optimisation::LinearProblemApi::ILinearProblem>&>(
      masterConst);
    auto problem = std::move(master);
    master = nullptr;
    return {std::move(problem), nullptr};
}

void SingleProblemGetterModeler::printProblems() const
{
    const auto& master = modeler_->masterProblem();
    const auto& subproblems = modeler_->subproblems();

    auto printProblem = [](const auto& problem, const char* name)
    {
        if (problem)
        {
            std::cout << name << ": " << problem->variableCount() << " variables, "
                      << problem->constraintCount() << " constraints\n";
        }
    };

    printProblem(master, "Master problem");
    for (size_t i = 0; i < subproblems.size(); ++i)
    {
        printProblem(subproblems[i], ("Subproblem " + std::to_string(i + 1)).c_str());
    }
}

} // namespace Antares::Solver::Implementation
