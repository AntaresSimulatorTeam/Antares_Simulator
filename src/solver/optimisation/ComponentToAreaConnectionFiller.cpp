// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/optimisation/ComponentToAreaConnectionFiller.h"

#include <antares/expressions/nodes/ExpressionsNodes.h>
#include "antares/exception/RuntimeError.hpp"
#include "antares/solver/optim-model-filler/ReadLinearExpressionVisitor.h"
#include "antares/solver/simulation/sim_structure_probleme_economique.h"

using namespace Antares::Optimisation;
using namespace Antares::Optimisation::LinearProblemApi;
using namespace Antares::ModelerStudy::SystemModel;
using namespace Antares::Expressions::Visitors;

std::map<std::string, unsigned> associateIndicesToAreas(const PROBLEME_HEBDO* problemeHebdo_)
{
    std::map<std::string, unsigned> areaIndices;
    unsigned index = 0;
    for (auto name: problemeHebdo_->NomsDesPays)
    {
        areaIndices.try_emplace(name, index);
        index++;
    }
    return areaIndices;
}

namespace Antares::Optimization
{
ComponentToAreaConnectionFiller::ComponentToAreaConnectionFiller(
  const PROBLEME_HEBDO* problemeHebdo,
  OptimEntityContainer& optimEntityContainer,
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
        for (const auto& areaId: component.portToAreaConnections() | std::views::values)
        {
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

std::vector<unsigned> balanceConstraintIndices(const PROBLEME_HEBDO* problemeHebdo,
                                               const FillContext& ctx,
                                               const unsigned& areaIndex)
{
    std::vector<unsigned> indices(ctx.getLocalNumberOfTimeSteps());
    for (unsigned h(0); h <= ctx.getLocalLastTimeStep(); ++h)
    {
        indices[h] = problemeHebdo->CorrespondanceCntNativesCntOptim[h]
                       .NumeroDeContrainteDesBilansPays[areaIndex];
    }
    return indices;
}

std::vector<unsigned> fictitiousLoadConstraintIndices(const PROBLEME_HEBDO* problemeHebdo,
                                                      const FillContext& ctx,
                                                      const unsigned& areaIndex)
{
    std::vector<unsigned> indices(ctx.getLocalNumberOfTimeSteps());
    for (unsigned h(0); h <= ctx.getLocalLastTimeStep(); ++h)
    {
        indices[h] = problemeHebdo->CorrespondanceCntNativesCntOptim[h]
                       .NumeroDeContraintePourEviterLesChargesFictives[areaIndex];
    }
    return indices;
}

std::vector<unsigned> maxUnsupEnergyConstraintIndices(const PROBLEME_HEBDO* problemeHebdo,
                                                      const FillContext& ctx,
                                                      const unsigned& areaIndex)
{
    std::vector<unsigned> indices(ctx.getLocalNumberOfTimeSteps());
    for (auto h(0); h <= ctx.getLocalLastTimeStep(); ++h)
    {
        indices[h] = problemeHebdo->CorrespondanceCntNativesCntOptim[h]
                       .NumeroDeContraintePourBornerLaDefaillance[areaIndex];
    }
    return indices;
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

std::vector<IMipConstraint*> ComponentToAreaConnectionFiller::fetchConstraints(
  const FillContext& ctx,
  const std::vector<unsigned>& constraintsIndices)
{
    auto& pb = optimEntityContainer_.Problem();
    std::vector<IMipConstraint*> constraints(ctx.getLocalNumberOfTimeSteps());
    for (auto h(0); h <= ctx.getLocalLastTimeStep(); ++h)
    {
        constraints[h] = pb.getConstraint(constraintsIndices[h]);
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
    return visitor.visitMergeDuplicates(expression).expandToSize(ctx.getLocalNumberOfTimeSteps());
}

void ComponentToAreaConnectionFiller::addPortContributionToLinearPb(
  const FillContext& ctx,
  const Component& component,
  const std::string& portId,
  const std::string& portField,
  const unsigned& areaIndex,
  const ConstraintIndicesHelper& helper)
{
    if (portField.empty())
    {
        // area connection does not specify this port field
        return;
    }

    // 1. GEMS side : get time-dependent linear expression at a component port field
    auto linearExpression = linearExpressionAtPortField(portId, portField, component, ctx);
    // 2. Legacy LP side : get the set of LP constraints to be modified
    std::vector<unsigned> constraintsIndices = helper(problemeHebdo_, ctx, areaIndex);
    auto constraints = fetchConstraints(ctx, constraintsIndices);
    // 3. Add the linear expression to LP constraints
    addExpressionToConstraint(linearExpression, ctx, constraints);
}

// This function is used to add terms (from GEMS modeler) to legacy linear problem constraints.
// For each constraint involved we add variable terms as negative terms
// and constant terms as positive terms.
// For details on reason why, see file :
//   docs/Architecture_Decision_Records/from-GEMS-to-legacy-linear-preblem.md
// Please update this file in case of change.

void ComponentToAreaConnectionFiller::addConstraints(const FillContext& ctx)
{
    for (const auto& component: modelerSystem_->Components())
    {
        for (auto [portId, areaId]: component.portToAreaConnections())
        {
            auto areaIndex = areaIndices_.at(areaId);
            const auto& areaConnection = component.areaConnectionAtPort(portId);
            const auto& port = component.findPort(portId, "");
            const auto& portType = port.Type();
            const auto& portAreaConnection = portType.areaConnection();

            if (areaConnection)
            {
                addPortContributionToLinearPb(ctx,
                                              component,
                                              portId,
                                              portAreaConnection->inject_to_balance,
                                              areaIndex,
                                              balanceConstraintIndices);
                addPortContributionToLinearPb(ctx,
                                              component,
                                              portId,
                                              portAreaConnection->spillage_bound,
                                              areaIndex,
                                              fictitiousLoadConstraintIndices);
                addPortContributionToLinearPb(ctx,
                                              component,
                                              portId,
                                              portAreaConnection->unsupplied_energy_bound,
                                              areaIndex,
                                              maxUnsupEnergyConstraintIndices);
            }
        }
    }
}

void ComponentToAreaConnectionFiller::addObjectives(const FillContext&)
{
    // nothing to do
}
} // namespace Antares::Optimization
