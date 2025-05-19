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
  const PROBLEME_SIMPLEXE_NOMME* problemeSimplexe,
  unsigned int nTimestepsInProblem,
  const ModelerStudy::SystemModel::System* modelerSystem,
  const VariableDictionary& modelerVariableDictionary):
    modelerSystem_(modelerSystem),
    modelerVariableDictionary_(modelerVariableDictionary),
    nTimestepsInProblem_(nTimestepsInProblem)
{
    parseConstraintIds(problemeSimplexe);
}

static std::string getLegacyConstraintName(const PROBLEME_SIMPLEXE_NOMME* problemeSimplexe,
                                           unsigned int index)
{
    // This should be in sync with LegacyFiller::GetConstraintName
    if (!problemeSimplexe->UseNamedProblems()
        || problemeSimplexe->ConstraintNames().at(index).empty())
    {
        return 'c' + std::to_string(index);
    }
    return problemeSimplexe->ConstraintNames().at(index);
}

void ComponentToAreaConnectionFiller::parseConstraintIds(
  const PROBLEME_SIMPLEXE_NOMME* problemeSimplexe)
{
    for (int idxRow = 0; idxRow < problemeSimplexe->NombreDeContraintes; ++idxRow)
    {
        boost::regex pattern(R"(AreaBalance::area<(.+)>::hour<(\d+)>)");
        boost::smatch matches;
        if (boost::regex_match(problemeSimplexe->ConstraintNames().at(idxRow), matches, pattern))
        {
            std::string areaId = matches[1].str();
            unsigned int ts = std::stoul(matches[2].str());
            auto id = getLegacyConstraintName(problemeSimplexe, idxRow);
            balanceConstraintPerAreaAndTimestep_.emplace(std::make_pair(areaId, ts), id);
        }
    }
}

void ComponentToAreaConnectionFiller::addVariables(ILinearProblem& pb,
                                                   ILinearProblemData& data,
                                                   FillContext& ctx)
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
    auto key = std::make_pair(areaId, ts % nTimestepsInProblem_);
    if (const auto it = balanceConstraintPerAreaAndTimestep_.find(key);
        it != balanceConstraintPerAreaAndTimestep_.end())
    {
        if (auto* ct = pb.lookupConstraint(it->second))
        {
            return ct;
        }
    }
    throw Error::RuntimeError("A component is connected to area \"" + areaId
                              + "\", that does not have a balance constraint defined for timestep "
                              + std::to_string(ts));
}

void ComponentToAreaConnectionFiller::addExpressionToConstraint(
  const LinearExpression& expression,
  IMipConstraint* areaBalanceConstraint) const
{
    // Contribution is added to the left-hand side of the constraint
    // We invert the sign bc modeler is in "gen>0, load<0" convention
    // legacy constraint is in "gen<0, load>0" convention
    for (const auto& [varKey, coef]: expression.coefPerVar())
    {
        auto* var = modelerVariableDictionary_[varKey];
        areaBalanceConstraint->setCoefficient(var, -coef);
    }
    areaBalanceConstraint->setBounds(areaBalanceConstraint->getLb() + expression.offset(),
                                     areaBalanceConstraint->getUb() + expression.offset());
}

void ComponentToAreaConnectionFiller::addComponentPortContributionToArea(
  ILinearProblem& pb,
  ILinearProblemData& data,
  const FillContext& ctx,
  const ModelerStudy::SystemModel::Component& component,
  const std::string& portId,
  const std::string& areaId)
{
    std::string injectionFieldId = getConnectionFieldId(component, portId);
    const Expressions::Visitors::EvaluationContext
      connectedComponentEvalContext(component.getParameterValues(), {}, data);
    ReadLinearExpressionVisitor visitor(connectedComponentEvalContext, ctx, component);
    auto timeDependentLinearExpression = visitor.dispatch(
      component.nodeAtPortField(portId, injectionFieldId));
    std::string lowerAreaId = areaId;
    boost::algorithm::to_lower(lowerAreaId);
    for (const auto& [ts, expression]: timeDependentLinearExpression.GetLinearExpressions())
    {
        IMipConstraint* areaBalanceConstraint = getBalanceConstraint(pb, lowerAreaId, ts);
        addExpressionToConstraint(expression, areaBalanceConstraint);
    }
}

void ComponentToAreaConnectionFiller::addConstraints(ILinearProblem& pb,
                                                     ILinearProblemData& data,
                                                     FillContext& ctx)
{
    for (auto component: modelerSystem_->Components() | std::ranges::views::values)
    {
        for (const auto& [portId, areaId]: component.portToAreaConnections())
        {
            addComponentPortContributionToArea(pb, data, ctx, component, portId, areaId);
        }
    }
}

void ComponentToAreaConnectionFiller::addObjective(ILinearProblem& pb,
                                                   ILinearProblemData& data,
                                                   FillContext& ctx)
{
    // nothing to do
}

} // namespace Antares::Optimization
