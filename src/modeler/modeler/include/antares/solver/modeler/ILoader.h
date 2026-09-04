// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once
#include <filesystem>

#include <antares/solver/modeler/parameters/modelerParameters.h>
#include "antares/solver/modeler/ModelerData.h"

namespace Antares::Solver
{
class ILoader
{
public:
    virtual ~ILoader() = default;

    virtual ModelerParameters loadParameters() = 0;

    virtual std::optional<ModelerData> loadAll() = 0;

    /// The study root directory, used to resolve study-relative file paths (e.g. playlist-file).
    virtual const std::filesystem::path& studyPath() const
    {
        static const std::filesystem::path empty;
        return empty;
    }
};
} // namespace Antares::Solver
