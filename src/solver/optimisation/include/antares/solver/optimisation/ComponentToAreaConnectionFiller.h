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

#include <ranges>

#include <boost/algorithm/string.hpp>

#include "antares/expressions/visitors/EvaluationContext.h"
#include "antares/logs/logs.h"
#include "antares/optimisation/linear-problem-api/IScenario.h"
#include "antares/optimisation/linear-problem-api/linearProblemFiller.h"
#include "antares/solver/optim-model-filler/ReadLinearExpressionVisitor.h"
#include "antares/solver/optim-model-filler/VariableDictionary.h"
#include "antares/solver/optim-model-filler/scenarioGroupRepo.h"
#include "antares/solver/simulation/sim_structure_probleme_economique.h"
#include "antares/study/runtime/runtime.h"
#include "antares/study/system-model/component.h"
#include "antares/study/system-model/system.h"

namespace Antares::Optimization
{

/**
 * \brief Fills the linear problem with constraints and variables related to component-to-area
 * connections.
 *
 * This class is responsible for adding variables, constraints, and objectives to the linear problem
 * based on the connections between components and areas in the Antares study.
 */
template<Optimisation::LinearProblemApi::SolverTag SolverTagType>
class ComponentToAreaConnectionFiller
    : public Optimisation::LinearProblemApi::LinearProblemFiller<SolverTagType>
{
public:
    explicit ComponentToAreaConnectionFiller(
      const PROBLEME_HEBDO* problemeHebdo,
      const VariableDictionary<typename SolverTagType::VariableType>& modelerVariableDictionary);
    void addVariables(Optimisation::LinearProblemApi::ILinearProblem<SolverTagType>& pb,
                      Optimisation::LinearProblemApi::ILinearProblemData& data,
                      Optimisation::LinearProblemApi::FillContext& ctx) override;
    void addConstraints(Optimisation::LinearProblemApi::ILinearProblem<SolverTagType>& pb,
                        Optimisation::LinearProblemApi::ILinearProblemData& data,
                        Optimisation::LinearProblemApi::FillContext& ctx) override;
    void addObjective(Optimisation::LinearProblemApi::ILinearProblem<SolverTagType>& pb,
                      Optimisation::LinearProblemApi::ILinearProblemData& data,
                      Optimisation::LinearProblemApi::FillContext& ctx) override;

private:
    const PROBLEME_HEBDO* problemeHebdo_;
    const ModelerStudy::SystemModel::System* modelerSystem_;
    const VariableDictionary<typename SolverTagType::VariableType>& modelerVariableDictionary_;

    std::map<std::string, unsigned> areaIndices_;

    Optimisation::LinearProblemApi::IMipConstraint<SolverTagType>* getBalanceConstraint(
      Optimisation::LinearProblemApi::ILinearProblem<SolverTagType>& pb,
      const std::string& areaId,
      unsigned ts) const;
    void addExpressionToConstraint(
      const LinearExpression& expression,
      Optimisation::LinearProblemApi::IMipConstraint<SolverTagType>* areaBalanceConstraint) const;
    void addComponentPortContributionToArea(
      Optimisation::LinearProblemApi::ILinearProblem<SolverTagType>& pb,
      Optimisation::LinearProblemApi::ILinearProblemData& data,
      const Optimisation::LinearProblemApi::FillContext& ctx,
      const ModelerStudy::SystemModel::Component& component,
      const std::string& portId,
      const std::string& areaId);
};

} // namespace Antares::Optimization

// Template implementations
namespace Antares::Optimization
{

namespace
{
std::string getConnectionFieldId(const ModelerStudy::SystemModel::Component& component,
                                 const std::string& portId)
{
    auto field = component.getModel()->Ports().at(portId).Type().AreaConnectionFieldId();
    if (!field.has_value())
    {
        throw std::runtime_error("Component \"" + component.Id()
                                 + "\" is connected to an area using a port type that has no "
                                   "area-connection field defined.");
    }
    return field.value();
}

// TODO remove and use proper scenario
class DefaultScenario: public Optimisation::LinearProblemApi::IScenario
{
public:
    using IScenario::IScenario;

    [[nodiscard]] unsigned int getData(unsigned int) const override
    {
        return 1; // Default rank for empty groupId
    }
};
} // namespace

template<Optimisation::LinearProblemApi::SolverTag SolverTagType>
ComponentToAreaConnectionFiller<SolverTagType>::ComponentToAreaConnectionFiller(
  const PROBLEME_HEBDO* problemeHebdo,
  const VariableDictionary<typename SolverTagType::VariableType>& modelerVariableDictionary):
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

template<Optimisation::LinearProblemApi::SolverTag SolverTagType>
void ComponentToAreaConnectionFiller<SolverTagType>::addVariables(
  Optimisation::LinearProblemApi::ILinearProblem<SolverTagType>&,
  Optimisation::LinearProblemApi::ILinearProblemData&,
  Optimisation::LinearProblemApi::FillContext&)
{
    // nothing to do
}

template<Optimisation::LinearProblemApi::SolverTag SolverTagType>
Optimisation::LinearProblemApi::IMipConstraint<SolverTagType>*
ComponentToAreaConnectionFiller<SolverTagType>::getBalanceConstraint(
  Optimisation::LinearProblemApi::ILinearProblem<SolverTagType>& pb,
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
    throw std::runtime_error("A component is connected to area \"" + areaId
                             + "\", that does not have a balance constraint defined for timestep "
                             + std::to_string(ts));
}

template<Optimisation::LinearProblemApi::SolverTag SolverTagType>
void ComponentToAreaConnectionFiller<SolverTagType>::addExpressionToConstraint(
  const LinearExpression& expression,
  Optimisation::LinearProblemApi::IMipConstraint<SolverTagType>* areaBalanceConstraint) const
{
    // Contribution is added to the left-hand side of the constraint
    // We invert the sign bc modeler is in "gen>0, load<0" convention
    // legacy constraint is in "gen<0, load>0" convention

    // Use optimized approach with variable indices for O(1) access
    for (const auto& [varIndex, coef]: expression.coefPerIndex())
    {
        auto* var = modelerVariableDictionary_.byIndex(varIndex); // O(1) access!
        areaBalanceConstraint->setCoefficient(var, -coef);
    }
    areaBalanceConstraint->setBounds(areaBalanceConstraint->getLb() + expression.offset(),
                                     areaBalanceConstraint->getUb() + expression.offset());
}

template<Optimisation::LinearProblemApi::SolverTag SolverTagType>
void ComponentToAreaConnectionFiller<SolverTagType>::addComponentPortContributionToArea(
  Optimisation::LinearProblemApi::ILinearProblem<SolverTagType>& pb,
  Optimisation::LinearProblemApi::ILinearProblemData& data,
  const Optimisation::LinearProblemApi::FillContext& ctx,
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
                                        modelerVariableDictionary_);
    auto timeDependentLinearExpression = visitor.dispatch(
      component.nodeAtPortField(portId, injectionFieldId));
    std::string lowerAreaId = areaId;
    boost::algorithm::to_lower(lowerAreaId);
    for (const auto& [ts, expression]: timeDependentLinearExpression.GetLinearExpressions())
    {
        auto* areaBalanceConstraint = getBalanceConstraint(pb, lowerAreaId, ts);
        addExpressionToConstraint(expression, areaBalanceConstraint);
    }
}

template<Optimisation::LinearProblemApi::SolverTag SolverTagType>
void ComponentToAreaConnectionFiller<SolverTagType>::addConstraints(
  Optimisation::LinearProblemApi::ILinearProblem<SolverTagType>& pb,
  Optimisation::LinearProblemApi::ILinearProblemData& data,
  Optimisation::LinearProblemApi::FillContext& ctx)
{
    for (const auto& component: modelerSystem_->Components() | std::ranges::views::values)
    {
        for (const auto& [portId, areaId]: component.portToAreaConnections())
        {
            addComponentPortContributionToArea(pb, data, ctx, component, portId, areaId);
        }
    }
}

template<Optimisation::LinearProblemApi::SolverTag SolverTagType>
void ComponentToAreaConnectionFiller<SolverTagType>::addObjective(
  Optimisation::LinearProblemApi::ILinearProblem<SolverTagType>&,
  Optimisation::LinearProblemApi::ILinearProblemData&,
  Optimisation::LinearProblemApi::FillContext&)
{
    // nothing to do
}

} // namespace Antares::Optimization
