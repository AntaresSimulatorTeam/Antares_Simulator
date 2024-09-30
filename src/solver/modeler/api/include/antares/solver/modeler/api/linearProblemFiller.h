/*
 * Copyright 2007-2024, RTE (https://www.rte-france.com)
 * See AUTHORS.txt
 * SPDX-License-Identifier: MPL-2.0
 * This file is part of Antares-Simulator,
 * Adequacy and Performance assessment for interconnected energy networks.
 *
 * Antares_Simulator is free software: you can redistribute it and/or modify
 * it under the terms of the Mozilla Public Licence 2.0 as published by
 * the Mozilla Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * Antares_Simulator is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * Mozilla Public Licence 2.0 for more details.
 *
 * You should have received a copy of the Mozilla Public Licence 2.0
 * along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
 */

#pragma once

#include <memory>

#include <antares/solver/modeler/api/linearProblem.h>
#include <antares/solver/modeler/api/linearProblemData.h>

namespace Antares::Solver::Modeler::Api
{

struct FillContext
{
    std::vector<unsigned> scenariosSelected;
    unsigned firstTimeStep;
    unsigned lastTimeStep;
};

class LinearProblemFiller
{
public:
    virtual void addVariables(ILinearProblem& pb, LinearProblemData& data) = 0;
    virtual void addConstraints(ILinearProblem& pb, LinearProblemData& data) = 0;
    virtual void addObjective(ILinearProblem& pb, LinearProblemData& data) = 0;
    virtual ~LinearProblemFiller() = default;
};

} // namespace Antares::Solver::Modeler::Api
