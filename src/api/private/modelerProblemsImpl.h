// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <filesystem>
#include <memory>

#include "antares/solver/modeler/ILoader.h"
#include "antares/solver/modeler/Modeler.h"
#include "antares/solver/modeler/loadFiles/Fileloader.h"
#include "antares/writer/table_format.h"

namespace Antares::Solver::Implementation
{

class ModelerProblems final
{
public:
    explicit ModelerProblems(const std::filesystem::path& modelerPath);

    // used for tests only
    ModelerProblems(ILoader& loader,
                    const std::filesystem::path& outputPath,
                    Antares::Writer::TableFormat tableFormat);

    void logSize() const;

private:
    std::unique_ptr<Modeler> modeler_;
    std::unique_ptr<LoadFiles::FileLoader> loader_;
};

} // namespace Antares::Solver::Implementation
