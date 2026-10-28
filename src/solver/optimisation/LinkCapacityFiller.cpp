// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/optimisation/LinkCapacityFiller.h"

#include <fmt/format.h>

#include "antares/exception/RuntimeError.hpp"
#include "antares/solver/modeler/ModelerData.h"
#include "antares/solver/optim-model-filler/ReadLinearExpressionVisitor.h"
#include "antares/solver/optimisation/variables/VariableManagerUtils.h"

using namespace Antares::LinearProblem;
using namespace Antares::LinearProblem::Api;
using namespace Antares::ModelerStudy::SystemModel;

namespace
{
constexpr double hurdleCostMargin = 0.01;

// The constraint reads: sign * flow - capacity(x) <= capacity constant, with capacity(x) the
// variable part of the capacity expression.
double solvedCapacity(const IMipConstraint& constraint, int flowVariable, const ILinearProblem& pb)
{
    double capacity = constraint.getUb();
    for (const auto& [varIndex, coef]: constraint.getCoefficients())
    {
        if (varIndex != flowVariable)
        {
            capacity -= coef * pb.getVariable(varIndex)->solutionValue();
        }
    }
    return capacity;
}
} // namespace

namespace Antares::Optimization
{
LinkCapacityFiller::LinkCapacityFiller(PROBLEME_HEBDO* problemeHebdo,
                                       OptimEntityContainer& optimEntityContainer,
                                       const ILinearProblemData* data,
                                       const ScenarioGroupRepository& scenarioGroupRepo):
    problemeHebdo_(problemeHebdo),
    modelerSystem_(problemeHebdo->modelerData->system.get()),
    optimEntityContainer_(optimEntityContainer),
    pb_(optimEntityContainer_.Problem()),
    data_(data),
    scenarioGroupRepo_(scenarioGroupRepo),
    variableManager_(VariableManagerFromProblemHebdo(problemeHebdo))
{
}

void LinkCapacityFiller::addVariables(const FillContext&)
{
    // nothing to do
}

void LinkCapacityFiller::addObjectives(const FillContext&)
{
    // nothing to do
}

int LinkCapacityFiller::getIntercoIndex(const std::string& from, const std::string& to) const
{
    bool reverseExists = false;
    for (uint32_t interco = 0; interco < problemeHebdo_->NombreDInterconnexions; ++interco)
    {
        // Area ids and link capacity connection ends are both lowercase
        const auto& fromArea = problemeHebdo_->NomsDesPays
                                 [problemeHebdo_->PaysOrigineDeLInterconnexion[interco]];
        const auto& toArea = problemeHebdo_->NomsDesPays
                               [problemeHebdo_->PaysExtremiteDeLInterconnexion[interco]];
        if (fromArea == from && toArea == to)
        {
            return static_cast<int>(interco);
        }
        if (fromArea == to && toArea == from)
        {
            reverseExists = true;
        }
    }
    if (reverseExists)
    {
        throw Error::RuntimeError(
          fmt::format("Unknown link '{} -> {}' in link capacity connection. A link with reverse "
                      "orientation '{} -> {}' exists; use Antares lexicographical direction.",
                      from,
                      to,
                      to,
                      from));
    }
    throw Error::RuntimeError(
      fmt::format("Unknown link '{} -> {}' in link capacity connection.", from, to));
}

void LinkCapacityFiller::addComponentPortContributionToLinkCapacity(
  const FillContext& ctx,
  const Component& component,
  const std::string& portId,
  const LinkComponent& linkConnection)
{
    const auto fields = component.getModel()
                          ->Ports()
                          .at(portId)
                          .Type()
                          .LinkCapacityConnectionFieldIds();
    if (!fields.has_value())
    {
        throw Error::RuntimeError(
          fmt::format("Component '{}' is connected to a legacy link using a port type that has no "
                      "link-capacity-connection fields defined.",
                      component.Id()));
    }

    const int interco = getIntercoIndex(linkConnection.from, linkConnection.to);

    ReadLinearExpressionVisitor visitor(optimEntityContainer_,
                                        ctx,
                                        component,
                                        data_,
                                        scenarioGroupRepo_);
    const auto directExpression = visitor.visitMergeDuplicates(
      component.nodeAtPortField(portId, fields->direct));
    const auto indirectExpression = visitor.visitMergeDuplicates(
      component.nodeAtPortField(portId, fields->indirect));

    const auto linkPrefix = fmt::format("LinkCapacity::from<{}>::to<{}>",
                                        linkConnection.from,
                                        linkConnection.to);
    for (auto localIndex(ctx.getLocalFirstTimeStep()); localIndex <= ctx.getLocalLastTimeStep();
         ++localIndex)
    {
        const int pdt = static_cast<int>(localIndex
                                         % problemeHebdo_->NombreDePasDeTempsPourUneOptimisation);
        const auto hourSuffix = fmt::format("::hour<{}>",
                                            pdt + problemeHebdo_->weekInTheYear * 168);

        const int directFlowIndex = variableManager_.DirectFlow(interco, pdt);
        auto* directFlow = pb_.getVariable(directFlowIndex);
        directFlow->setLb(-pb_.infinity());
        directFlow->setUb(pb_.infinity());

        auto* upperCt = addCapacityConstraint(directFlow,
                                              1.,
                                              directExpression[localIndex],
                                              0.,
                                              linkPrefix + "::NTCDirectUpper" + hourSuffix);
        auto* lowerCt = addCapacityConstraint(directFlow,
                                              -1.,
                                              indirectExpression[localIndex],
                                              0.,
                                              linkPrefix + "::NTCDirectLower" + hourSuffix);
        problemeHebdo_->linkCapacityConstraints.push_back(
          {.interco = static_cast<uint32_t>(interco),
           .pdt = pdt,
           .flowVariable = directFlowIndex,
           .direct = upperCt,
           .indirect = lowerCt});

        if (problemeHebdo_->CoutDeTransport[interco].IntercoGereeAvecDesCouts)
        {
            auto* directCost = pb_.getVariable(variableManager_.PositiveDirectFlow(interco, pdt));
            directCost->setLb(0.);
            directCost->setUb(pb_.infinity());
            auto* indirectCost = pb_.getVariable(
              variableManager_.PositiveIndirectFlow(interco, pdt));
            indirectCost->setLb(0.);
            indirectCost->setUb(pb_.infinity());

            // Same margin as the legacy bounds: the capacity dual must stay on the flow
            // constraints, not be split with these degenerate ones.
            addCapacityConstraint(directCost,
                                  1.,
                                  directExpression[localIndex],
                                  hurdleCostMargin,
                                  linkPrefix + "::IntercoDirectCostUpper" + hourSuffix);
            addCapacityConstraint(indirectCost,
                                  1.,
                                  indirectExpression[localIndex],
                                  hurdleCostMargin,
                                  linkPrefix + "::IntercoIndirectCostUpper" + hourSuffix);
        }
    }
}

IMipConstraint* LinkCapacityFiller::addCapacityConstraint(IMipVariable* variable,
                                                          double sign,
                                                          const LinearExpression& capacity,
                                                          double margin,
                                                          const std::string& name)
{
    // sign * variable <= capacity + margin
    auto* constraint = pb_.addConstraint(-pb_.infinity(), capacity.constant() + margin, name);
    constraint->setCoefficient(variable, sign);
    const auto& solverVariables = pb_.getVariables();
    for (const auto& [varIndex, coef]: capacity)
    {
        constraint->setCoefficient(solverVariables[varIndex].get(), -coef);
    }
    return constraint;
}

void applyLinkCapacityResults(PROBLEME_HEBDO& problemeHebdo,
                              const ILinearProblem& pb,
                              int NumIntervalle)
{
    const int firstPdt = NumIntervalle * problemeHebdo.NombreDePasDeTempsPourUneOptimisation;
    auto& reducedCosts = problemeHebdo.ProblemeAResoudre->CoutsReduits;
    for (const auto& link: problemeHebdo.linkCapacityConstraints)
    {
        auto& ntc = problemeHebdo.ValeursDeNTC[firstPdt + link.pdt];
        ntc.ValeurDeNTCOrigineVersExtremite[link.interco] = solvedCapacity(*link.direct,
                                                                           link.flowVariable,
                                                                           pb);
        ntc.ValeurDeNTCExtremiteVersOrigine[link.interco] = solvedCapacity(*link.indirect,
                                                                           link.flowVariable,
                                                                           pb);
        // Duals are not extracted from a MIP, see extract_from_MPSolver
        if (pb.isLP())
        {
            // Equivalent reduced cost of the flow bounds: the flow coefficient is +1 in the
            // direct constraint and -1 in the indirect one.
            reducedCosts[link.flowVariable] = link.direct->dual() - link.indirect->dual();
        }
    }
}

void LinkCapacityFiller::addConstraints(const FillContext& ctx)
{
    for (const auto& component: modelerSystem_->Components())
    {
        for (const auto& [portId, linkConnection]: component.portToLinkCapacityConnections())
        {
            addComponentPortContributionToLinkCapacity(ctx, component, portId, linkConnection);
        }
    }
}

} // namespace Antares::Optimization
