// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/optimisation/LinkCapacityFiller.h"

#include <fmt/format.h>

#include <boost/algorithm/string/case_conv.hpp>

#include "antares/exception/RuntimeError.hpp"
#include "antares/solver/modeler/ModelerData.h"
#include "antares/solver/optim-model-filler/ReadLinearExpressionVisitor.h"
#include "antares/solver/optimisation/variables/VariableManagerUtils.h"

using namespace Antares::LinearProblem;
using namespace Antares::LinearProblem::Api;
using namespace Antares::ModelerStudy::SystemModel;

namespace
{
std::string toLowerCase(std::string value)
{
    boost::algorithm::to_lower(value);
    return value;
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
        const auto fromArea = toLowerCase(
          problemeHebdo_->NomsDesPays[problemeHebdo_->PaysOrigineDeLInterconnexion[interco]]);
        const auto toArea = toLowerCase(
          problemeHebdo_->NomsDesPays[problemeHebdo_->PaysExtremiteDeLInterconnexion[interco]]);
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
    const auto& solverVariables = pb_.getVariables();
    for (auto localIndex(ctx.getLocalFirstTimeStep()); localIndex <= ctx.getLocalLastTimeStep();
         ++localIndex)
    {
        auto pdt = localIndex % problemeHebdo_->NombreDePasDeTempsPourUneOptimisation;
        const auto hourSuffix = fmt::format("::hour<{}>",
                                            pdt + problemeHebdo_->weekInTheYear * 168);

        auto* directFlow = pb_.getVariable(variableManager_.DirectFlow(interco, pdt));
        directFlow->setLb(-pb_.infinity());
        directFlow->setUb(pb_.infinity());

        auto* upperCt = pb_.addConstraint(-pb_.infinity(),
                                          directExpression[localIndex].constant(),
                                          linkPrefix + "::NTCDirectUpper" + hourSuffix);
        upperCt->setCoefficient(directFlow, 1.0);
        for (const auto& [varIndex, coef]: directExpression[localIndex])
        {
            upperCt->setCoefficient(solverVariables[varIndex].get(), -coef);
        }

        auto* lowerCt = pb_.addConstraint(-pb_.infinity(),
                                          indirectExpression[localIndex].constant(),
                                          linkPrefix + "::NTCDirectLower" + hourSuffix);
        lowerCt->setCoefficient(directFlow, -1.0);
        for (const auto& [varIndex, coef]: indirectExpression[localIndex])
        {
            lowerCt->setCoefficient(solverVariables[varIndex].get(), -coef);
        }

        if (problemeHebdo_->CoutDeTransport[interco].IntercoGereeAvecDesCouts)
        {
            auto* directCost = pb_.getVariable(variableManager_.PositiveDirectFlow(interco, pdt));
            directCost->setLb(0.);
            directCost->setUb(pb_.infinity());
            auto* directCostCt = pb_.addConstraint(-pb_.infinity(),
                                                   directExpression[localIndex].constant(),
                                                   linkPrefix + "::IntercoDirectCostUpper"
                                                     + hourSuffix);
            directCostCt->setCoefficient(directCost, 1.0);
            for (const auto& [varIndex, coef]: directExpression[localIndex])
            {
                directCostCt->setCoefficient(solverVariables[varIndex].get(), -coef);
            }

            auto* indirectCost = pb_.getVariable(
              variableManager_.PositiveIndirectFlow(interco, pdt));
            indirectCost->setLb(0.);
            indirectCost->setUb(pb_.infinity());
            auto* indirectCostCt = pb_.addConstraint(-pb_.infinity(),
                                                     indirectExpression[localIndex].constant(),
                                                     linkPrefix + "::IntercoIndirectCostUpper"
                                                       + hourSuffix);
            indirectCostCt->setCoefficient(indirectCost, 1.0);
            for (const auto& [varIndex, coef]: indirectExpression[localIndex])
            {
                indirectCostCt->setCoefficient(solverVariables[varIndex].get(), -coef);
            }
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
