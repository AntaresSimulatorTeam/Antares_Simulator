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

#include <antares/solver/modeler/api/linearProblemFiller.h>
#include <antares/study/system-model/component.h>
#include "antares/solver/expressions/visitors/EvaluationContext.h"

namespace Antares::Study::SystemModel
{
class Component;
}

namespace Antares::Optimization
{
/**
 * Component filler
 * Implements LinearProbleFiller interface.
 * Fills a LinearProblem with variables, constraints, and objective coefficients of a Component
 */
class ComponentFiller: public Solver::Modeler::Api::LinearProblemFiller
{
public:
    ComponentFiller() = delete;
    ComponentFiller(ComponentFiller& other) = delete;
    /// Create a ComponentFiller for a Component
    explicit ComponentFiller(const Study::SystemModel::Component& component);

    void addVariables(Solver::Modeler::Api::ILinearProblem& pb,
                      Solver::Modeler::Api::LinearProblemData& data,
                      Solver::Modeler::Api::FillContext& ctx) override;
    void addConstraints(Solver::Modeler::Api::ILinearProblem& pb,
                        Solver::Modeler::Api::LinearProblemData& data,
                        Solver::Modeler::Api::FillContext& ctx) override;
    void addObjective(Solver::Modeler::Api::ILinearProblem& pb,
                      Solver::Modeler::Api::LinearProblemData& data,
                      Solver::Modeler::Api::FillContext& ctx) override;

private:
    const Study::SystemModel::Component& component_;
    Solver::Visitors::EvaluationContext evaluationContext_;
};
} // namespace Antares::Optimization
