// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/optimisation/ComponentToAreaResultFiller.h"

#include <boost/algorithm/string/case_conv.hpp>

#include "antares/solver/optim-model-filler/ReadLinearExpressionVisitor.h"

using namespace Antares::Optimisation;
using namespace Antares::Optimisation::LinearProblemApi;

namespace Antares::Optimization
{

ComponentToAreaResultFiller::ComponentToAreaResultFiller(
  PROBLEME_HEBDO* problemeHebdo,
  const OptimEntityContainer& optimEntityContainer):
    problemeHebdo_(problemeHebdo),
    modelerSystem_(problemeHebdo->modelerData->system.get()),
    optimEntityContainer_(optimEntityContainer)
{
    int i = 0;
    for (auto name: problemeHebdo_->NomsDesPays)
    {
        areaIndices_[name] = i++;
    }
}

double ComponentToAreaResultFiller::evaluateExpressionAtTimestep(
  const TimeDependentLinearExpression& expr,
  unsigned ts) const
{
    const auto& solverVariables = optimEntityContainer_.getVariables();
    const LinearExpression& linExpr = expr[ts];
    double value = linExpr.constant();
    for (const auto& [index, coef]: linExpr)
    {
        value += coef * solverVariables.at(index)->solutionValue();
    }
    return value;
}

void ComponentToAreaResultFiller::fillAreaContributions(const FillContext& ctx)
{
    for (const auto& component: modelerSystem_->Components())
    {
        for (const auto& [portId, areaId]: component.portToAreaConnections())
        {
            auto field = component.getModel()->Ports().at(portId).Type().AreaConnectionFieldId();
            if (!field.has_value())
            {
                continue;
            }
            const std::string& injectionFieldId = field.value();

            ReadLinearExpressionVisitor visitor(optimEntityContainer_, ctx, component);
            auto linearExpression = visitor.visitMergeDuplicates(
              component.nodeAtPortField(portId, injectionFieldId));

            std::string lowerAreaId = areaId;
            boost::algorithm::to_lower(lowerAreaId);

            auto it = areaIndices_.find(lowerAreaId);
            if (it == areaIndices_.end())
            {
                continue;
            }
            int areaIndex = it->second;

            for (auto ts = ctx.getLocalFirstTimeStep(); ts <= ctx.getLocalLastTimeStep(); ++ts)
            {
                problemeHebdo_->ResultatsHoraires[areaIndex]
                  .ValeursHorairesNetechangeModeler[ts] = evaluateExpressionAtTimestep(
                  linearExpression,
                  ts);
            }
        }
    }
}

} // namespace Antares::Optimization
