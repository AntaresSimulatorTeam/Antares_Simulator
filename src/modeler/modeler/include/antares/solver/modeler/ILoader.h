// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once
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
};
} // namespace Antares::Solver
