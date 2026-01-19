// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once
#include <filesystem>

#include <antares/solver/modeler/ILoader.h>

namespace Antares::Solver::LoadFiles
{
class FileLoader final: public ILoader
{
public:
    explicit FileLoader(std::filesystem::path studyPath);

    ModelerParameters loadParameters() override;

    Antares::Modeler::Data loadAll() override;

    std::filesystem::path studyPath_;
};
} // namespace Antares::Solver::LoadFiles
