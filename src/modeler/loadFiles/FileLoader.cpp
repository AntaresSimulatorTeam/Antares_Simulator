// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/modeler/loadFiles/Fileloader.h"

#include <utility>

#include "antares/solver/modeler/loadFiles/loadFiles.h"

namespace Antares::Solver::LoadFiles
{
FileLoader::FileLoader(std::filesystem::path studyPath):
    studyPath_(std::move(studyPath))
{
}

ModelerParameters FileLoader::loadParameters()
{
    return LoadFiles::loadParameters(studyPath_);
}

std::optional<ModelerData> FileLoader::loadAll()
{
    return LoadFiles::loadAll(studyPath_);
}
} // namespace Antares::Solver::LoadFiles
