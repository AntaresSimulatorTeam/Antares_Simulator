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
#include <boost/regex.hpp>

#include <antares/expressions/nodes/ExpressionsNodes.h>
#include "antares/exception/RuntimeError.hpp"
#include "antares/solver/optim-model-filler/ReadLinearExpressionVisitor.h"
#include "antares/solver/simulation/sim_structure_probleme_economique.h"

using namespace Antares::Optimisation::LinearProblemApi;

namespace Antares::Optimization
{
ComponentToAreaConnectionFiller::ComponentToAreaConnectionFiller(
  const PROBLEME_HEBDO* problemeHebdo,
  const std::vector<std::vector<IMipVariable*>>& modelerVariableDictionary):
    problemeHebdo_(problemeHebdo),
    modelerSystem_(problemeHebdo->modelerSystem),
    modelerVariableDictionary_(modelerVariableDictionary)
{
    int i = 0;
    for (auto name: problemeHebdo_->NomsDesPays)
    {
        areaIndices_[name] = i++;
    }
}

void ComponentToAreaConnectionFiller::addVariables(ILinearProblem&,
                                                   ILinearProblemData&,
                                                   const FillContext&)
{
    // nothing to do
}

static std::string getConnectionFieldId(const ModelerStudy::SystemModel::Component& component,
                                        const std::string& portId)
{
    auto field = component.getModel()->Ports().at(portId).Type().AreaConnectionFieldId();
    if (!field.has_value())
    {
        throw Error::RuntimeError("Component \"" + component.Id()
                                  + "\" is connected to an area using a port type that has no "
                                    "area-connection field defined.");
    }
    return field.value();
}

IMipConstraint* ComponentToAreaConnectionFiller::getBalanceConstraint(ILinearProblem& pb,
                                                                      const std::string& areaId,
                                                                      unsigned ts) const
{
    auto pdt = ts % problemeHebdo_->NombreDePasDeTempsPourUneOptimisation;
    if (const auto it = areaIndices_.find(areaId); it != areaIndices_.end())
    {
        auto contraintIndex = problemeHebdo_->CorrespondanceCntNativesCntOptim[pdt]
                                .NumeroDeContrainteDesBilansPays[it->second];
        if (auto* ct = pb.getConstraint(contraintIndex))
        {
            return ct;
        }
    }
    throw Error::RuntimeError("A component is connected to area \"" + areaId
                              + "\", that does not have a balance constraint defined for timestep "
                              + std::to_string(ts));
}

void ComponentToAreaConnectionFiller::addExpressionToConstraint(
  ILinearProblem& pb,
  const LinearExpression& linearExpression,
  const FillContext& ctx,
  const std::string& areaId) const
{
    // Contribution is added to the left-hand side of the constraint
    // We invert the sign bc modeler is in "gen>0, load<0" convention
    // legacy constraint is in "gen<0, load>0" convention
    // for (const auto& [varKey, coef]: coeffPerVar)
    // {
    //     auto* var = modelerVariableDictionary_[varKey];
    //     areaBalanceConstraint->setCoefficient(var, -coef);
    // }
    std::string lowerAreaId = areaId;
    boost::algorithm::to_lower(lowerAreaId);
    for (auto localIndex(ctx.getLocalFirstTimeStep()); localIndex <= ctx.getLocalLastTimeStep();
         ++localIndex)
    {
        IMipConstraint* areaBalanceConstraint = getBalanceConstraint(pb, lowerAreaId, localIndex);
        // addExpressionToConstraint(expression, areaBalanceConstraint);
        for (auto modelVarIndex = 0; modelVarIndex < linearExpression.coefPerVar().size();
             ++modelVarIndex)
        {
            const auto& coeffPerVar = linearExpression.coefPerVar();
            const auto& variables = modelerVariableDictionary_.at(modelVarIndex);
            if (variables.size() > localIndex)
            {
                const auto& coefficients = coeffPerVar.at(modelVarIndex);
                areaBalanceConstraint->setCoefficient(variables.at(localIndex),
                                                      coefficients.singleValueOrAtIndex(
                                                        localIndex));
            }
        }
        areaBalanceConstraint->setBounds(
          areaBalanceConstraint->getLb()
            + linearExpression.offset().singleValueOrAtIndex(localIndex),
          areaBalanceConstraint->getUb()
            + linearExpression.offset().singleValueOrAtIndex(localIndex));
    }
}

// TODO remove and ue proper scenario
class DefaultScenario: public IScenario
{
public:
    using IScenario::IScenario;

    [[nodiscard]] TimeSeriesNumber getData(Year) const override
    {
        return 1; // Default rank for empty groupId
    }
};

void ComponentToAreaConnectionFiller::addComponentPortContributionToArea(
  ILinearProblem& pb,
  ILinearProblemData& data,
  const FillContext& ctx,
  const ModelerStudy::SystemModel::Component& component,
  const std::string& portId,
  const std::string& areaId)
{
    std::string injectionFieldId = getConnectionFieldId(component, portId);
    DefaultScenario defaultScenario("empty"); // TODO default ?
    const Expressions::Visitors::EvaluationContext
      connectedComponentEvalContext(component.getParameterValues(), {}, data, defaultScenario);
    ReadLinearExpressionVisitor visitor(connectedComponentEvalContext,
                                        ctx,
                                        component,
                                        modelerVariableDictionary_.size());
    auto linearExpression = visitor.dispatch(component.nodeAtPortField(portId, injectionFieldId));
    addExpressionToConstraint(pb, linearExpression, ctx, areaId);
}

void ComponentToAreaConnectionFiller::addConstraints(ILinearProblem& pb,
                                                     ILinearProblemData& data,
                                                     const FillContext& ctx)
{
    for (const auto& component: modelerSystem_->Components() | std::ranges::views::values)
    {
        for (const auto& [portId, areaId]: component.portToAreaConnections())
        {
            addComponentPortContributionToArea(pb, data, ctx, component, portId, areaId);
        }
    }
}

void ComponentToAreaConnectionFiller::addObjective(ILinearProblem&,
                                                   ILinearProblemData&,
                                                   const FillContext&)
{
    // nothing to do
}
} // namespace Antares::Optimization
