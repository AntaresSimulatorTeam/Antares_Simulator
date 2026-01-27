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
  const std::vector<unsigned>& constraintIndices) const
{
    // Contribution is added to the left-hand side of the constraint
    // We invert the sign bc modeler is in "gen>0, load<0" convention
    // legacy constraint is in "gen<0, load>0" convention
    auto& pb = optimEntityContainer_.Problem();
    const auto& solverVariables = optimEntityContainer_.getVariables();

    for (auto h(0); h <= ctx.getLocalLastTimeStep(); ++h)
    {
        IMipConstraint* balanceConstraint = pb.getConstraint(constraintIndices[h]);

        for (const auto& [index, coef]: linearExpression[h])
        {
            balanceConstraint->setCoefficient(solverVariables.at(index).get(), -coef);
        }

        double constant = linearExpression[h].constant();
        balanceConstraint->setBounds(balanceConstraint->getLb() + constant,
                                     balanceConstraint->getUb() + constant);
    }
}

std::vector<unsigned> ComponentToAreaConnectionFiller::balanceConstraintsIndices(
  const FillContext& ctx,
  const unsigned& areaIndex)
{
    std::vector<unsigned> constraintIndices(ctx.getLocalLastTimeStep() + 1);
    for (auto h(0); h <= ctx.getLocalLastTimeStep(); ++h)
    {
        auto ts = h % problemeHebdo_->NombreDePasDeTempsPourUneOptimisation;
        constraintIndices[h] = balanceConstraintIndex(areaIndex, ts);
    }
    return constraintIndices;
}

void ComponentToAreaConnectionFiller::addComponentPortContributionToArea(const FillContext& ctx,
                                                                         const Component& component,
                                                                         const std::string& portId,
                                                                         const unsigned& areaIndex)
{
    std::string injectionFieldId = componentInjectionField(component, portId);
    if (injectionFieldId.empty())
    {
        return;
    }

    Nodes::Node* expression = component.nodeAtPortField(portId, injectionFieldId);

    ReadLinearExpressionVisitor visitor(optimEntityContainer_, ctx, component);
    auto linearExpression = visitor.visitMergeDuplicates(expression);

    std::vector<unsigned> constraintIndices = balanceConstraintsIndices(ctx, areaIndex);

    addExpressionToConstraint(linearExpression, ctx, constraintIndices);
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
