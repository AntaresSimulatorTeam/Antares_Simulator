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

#include "antares/solver/optimisation/ComponentToAreaConnectionFiller.h"

#include <boost/algorithm/string/case_conv.hpp>

#include <antares/expressions/nodes/ExpressionsNodes.h>
#include "antares/exception/RuntimeError.hpp"
#include "antares/expressions/visitors/EvalVisitor.h"
#include "antares/solver/optim-model-filler/ReadLinearExpressionVisitor.h"
#include "antares/solver/simulation/sim_structure_probleme_economique.h"

using namespace Antares::Optimisation;
using namespace Antares::Optimisation::LinearProblemApi;
using namespace Antares::ModelerStudy::SystemModel;
using namespace Antares::Expressions::Visitors;

std::string componentInjectionField(const Component& component, const std::string& portId)
{
    return component.getModel()->Ports().at(portId).Type().areaConnection()->injection;
}

std::string componentToAreaBoundField(const Component& component, const std::string& portId)
{
    return component.getModel()->Ports().at(portId).Type().areaConnection()->to_area_bound;
}

std::map<std::string, unsigned> associateIndicesToAreas(const PROBLEME_HEBDO* problemeHebdo_)
{
    std::map<std::string, unsigned> areaIndices;
    unsigned index = 0;
    for (auto name: problemeHebdo_->NomsDesPays)
    {
        areaIndices.insert({name, index++});
    }
    return areaIndices;
}

namespace Antares::Optimization
{
ComponentToAreaConnectionFiller::ComponentToAreaConnectionFiller(
  const PROBLEME_HEBDO* problemeHebdo,
  OptimEntityContainer& optimEntityContainer,
  const ILinearProblemData& linearProblemData,
  const Optimisation::ScenarioGroupRepository& scenarioGroupRepository):
    problemeHebdo_(problemeHebdo),
    modelerSystem_(problemeHebdo->modelerData->system.get()),
    optimEntityContainer_(optimEntityContainer)
{
    areaIndices_ = associateIndicesToAreas(problemeHebdo_);
    checkAreasFromConnexionsExist();
}

void ComponentToAreaConnectionFiller::checkAreasFromConnexionsExist()
{
    for (const auto& component: modelerSystem_->Components())
    {
        for (auto [portId, areaId]: component.portToAreaConnections())
        {
            boost::algorithm::to_lower(areaId);
            if (const auto it = areaIndices_.find(areaId); it == areaIndices_.end())
            {
                std::string errMsg = "Component '" + component.Id() + "' is connected ";
                errMsg += "to a non existing area : " + areaId;
                throw Error::RuntimeError(errMsg);
            }
        }
    }
}

void ComponentToAreaConnectionFiller::addVariables(const FillContext& ctx)
{
}

unsigned ComponentToAreaConnectionFiller::balanceConstraintIndex(const unsigned& areaIndex,
                                                                 unsigned ts) const
{
    return problemeHebdo_->CorrespondanceCntNativesCntOptim[ts]
      .NumeroDeContrainteDesBilansPays[areaIndex];
}

void ComponentToAreaConnectionFiller::addExpressionToConstraint(
  const TimeDependentLinearExpression& linearExpression,
  const FillContext& ctx,
  const std::vector<IMipConstraint*>& constraints) const
{
    const auto& solverVariables = optimEntityContainer_.getVariables();

    for (auto h(0); h <= ctx.getLocalLastTimeStep(); ++h)
    {
        IMipConstraint* constraint = constraints[h];
        for (const auto& [index, coef]: linearExpression[h])
        {
            constraint->setCoefficient(solverVariables.at(index).get(), -coef);
        }

        double c = linearExpression[h].constant();
        constraint->setBounds(constraint->getLb() + c, constraint->getUb() + c);
    }
}

std::vector<IMipConstraint*> ComponentToAreaConnectionFiller::balanceConstraints(
  const FillContext& ctx,
  const unsigned& areaIndex)
{
    auto& pb = optimEntityContainer_.Problem();
    std::vector<IMipConstraint*> constraints(ctx.getLocalLastTimeStep() + 1);
    for (auto h(0); h <= ctx.getLocalLastTimeStep(); ++h)
    {
        auto ts = h % problemeHebdo_->NombreDePasDeTempsPourUneOptimisation;
        unsigned constraintIndex = balanceConstraintIndex(areaIndex, ts);
        constraints[h] = pb.getConstraint(constraintIndex);
    }
    return constraints;
}

TimeDependentLinearExpression ComponentToAreaConnectionFiller::linearExpressionAtPortField(
  const std::string& portId,
  const std::string& fieldId,
  const Component& component,
  const FillContext& ctx)
{
    ReadLinearExpressionVisitor visitor(optimEntityContainer_, ctx, component);

    Nodes::Node* expression = component.nodeAtPortField(portId, fieldId);
    return visitor.visitMergeDuplicates(expression);
}

void ComponentToAreaConnectionFiller::addComponentPortContributionToArea(const FillContext& ctx,
                                                                         const Component& component,
                                                                         const std::string& portId,
                                                                         const unsigned& areaIndex)
{
    std::string portFieldId = componentInjectionField(component, portId);
    if (portFieldId.empty())
    {
        return;
    }

    // 1. Get time-dependent linear expression at a component port field
    auto linearExpression = linearExpressionAtPortField(portId, portFieldId, component, ctx);
    // 2. Get the set of LP constraints to be modified with previous linear expression
    auto constraints = balanceConstraints(ctx, areaIndex);
    // 3. Add the linear expression to LP constraints
    addExpressionToConstraint(linearExpression, ctx, constraints);
}

void ComponentToAreaConnectionFiller::addConstraints(const FillContext& ctx)
{
    for (const auto& component: modelerSystem_->Components())
    {
        for (auto [portId, areaId]: component.portToAreaConnections())
        {
            boost::algorithm::to_lower(areaId);
            auto areaIndex = areaIndices_.at(areaId);
            addComponentPortContributionToArea(ctx, component, portId, areaIndex);
        }
    }
}

void ComponentToAreaConnectionFiller::addObjectives(const FillContext&)
{
    // nothing to do
}
} // namespace Antares::Optimization
