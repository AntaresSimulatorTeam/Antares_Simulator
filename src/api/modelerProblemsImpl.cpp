// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "modelerProblemsImpl.h"

namespace Antares::Solver::Implementation
{

ModelerProblems::ModelerProblems(const std::filesystem::path& studyPath)
{
    loader_ = std::make_unique<LoadFiles::FileLoader>(studyPath);
    fs::path outputPath = makeOutputPath(studyPath);

    modeler_ = std::make_unique<Modeler>(*loader_, outputPath, TableFormat::CSV);
    modeler_->buildProblemsAndWriteMps();
}

} // namespace Antares::Solver::Implementation
