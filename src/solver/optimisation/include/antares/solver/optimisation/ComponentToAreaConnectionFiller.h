/*
 * Copyright 2007-2025, RTE (https://www.rte-france.com)
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

#include "antares/optimisation/linear-problem-api/linearProblemFiller.h"
#include "antares/solver/optim-model-filler/LinearExpression.h"
#include "antares/solver/simulation/sim_structure_probleme_economique.h"
#include "antares/study/system-model/system.h"

namespace Antares::Optimisation
{
namespace LinearProblemApi
{
class ILinearProblem;
class IMipConstraint;
} // namespace LinearProblemApi
class OptimEntityContainer;
} // namespace Antares::Optimisation

namespace Antares::Optimization
{

/**
 * \brief Fills the linear problem with constraints and variables related to component-to-area
 * connections.
 *
 * This class is responsible for adding variables, constraints, and objectives to the linear problem
 * based on the connections between components and areas in the Antares study.
 */
class ComponentToAreaConnectionFiller final
    : public Optimisation::LinearProblemApi::LinearProblemFiller
{
public:
    explicit ComponentToAreaConnectionFiller(
      const PROBLEME_HEBDO* problemeHebdo,
      Optimisation::OptimEntityContainer& variableContainer,
      const Optimisation::LinearProblemApi::ILinearProblemData& linearProblemData,
      const Optimisation::ScenarioGroupRepository& scenarioGroupRepository);
    void addVariables(const Optimisation::LinearProblemApi::FillContext& ctx) override;
    void addConstraints(const Optimisation::LinearProblemApi::FillContext& ctx) override;
    void addObjective(const Optimisation::LinearProblemApi::FillContext& ctx) override;

private:
    const PROBLEME_HEBDO* problemeHebdo_;
    const ModelerStudy::SystemModel::System* modelerSystem_;
    Optimisation::OptimEntityContainer& optimEntityContainer_;

    std::map<std::string, unsigned> areaIndices_;

    Optimisation::LinearProblemApi::IMipConstraint* getBalanceConstraint(
      Optimisation::LinearProblemApi::ILinearProblem& pb,
      const std::string& areaId,
      unsigned ts) const;
    void addExpressionToConstraint(
      Optimisation::LinearProblemApi::ILinearProblem& pb,
      const Antares::Optimization::TimeDependentLinearExpression& linearExpression,
      const Optimisation::LinearProblemApi::FillContext& ctx,
      const std::string& areaId) const;
    void addComponentPortContributionToArea(Optimisation::LinearProblemApi::ILinearProblem& pb,
                                            const Optimisation::LinearProblemApi::FillContext& ctx,
                                            const ModelerStudy::SystemModel::Component& component,
                                            const std::string& portId,
                                            const std::string& areaId);
};

} // namespace Antares::Optimization
