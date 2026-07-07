// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "modelerProblemsImpl.h"

#include "antares/logs/logs.h"
#include "antares/utils/utils.h"

namespace Antares::Solver::Implementation
{

ModelerProblems::ModelerProblems(const std::filesystem::path& studyPath)
{
    loader_ = std::make_unique<LoadFiles::FileLoader>(studyPath);
    fs::path outputPath = makeOutputPath(studyPath);

    modeler_ = std::make_unique<Modeler>(*loader_, outputPath, TableFormat::CSV);
    modeler_->buildProblems();
    modeler_->exportMps();
    modeler_->exportStructureFile();
}

void logProblemSize(const Optimisation::LinearProblemApi::ILinearProblem* problem,
                    const std::string& name)
{
    logs.info() << name << ": " << problem->variableCount() << " variables, "
                << problem->constraintCount() << " constraints";
}

void ModelerProblems::logSize() const
{
    const auto& master = modeler_->masterProblem();
    const auto& subproblems = modeler_->subproblems();

    logProblemSize(master.get(), "Master problem");
    for (size_t i = 0; i < subproblems.size(); ++i)
    {
        logProblemSize(subproblems[i].get(), ("Subproblem " + std::to_string(i + 1)).c_str());
    }
}

} // namespace Antares::Solver::Implementation
