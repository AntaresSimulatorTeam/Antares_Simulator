// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include <antares/solver/optim-model-filler/EvaluationContextProvider.h>

namespace Antares::Optimisation
{
EvaluationContextProvider::EvaluationContextProvider(
  const LinearProblemApi::ILinearProblemData& data,
  const ScenarioGroupRepository& scenarioGroupRepository,
  std::map<std::string, double> variables):
    data_(data),
    scenarioGroupRepository_(scenarioGroupRepository),
    variables_(variables)
{
}

Expressions::Visitors::EvaluationContext EvaluationContextProvider::provide(
  const ModelerStudy::SystemModel::Component& component) const
{
    return Expressions::Visitors::EvaluationContext(component.getParameterValues(),
                                                    variables_,
                                                    data_,
                                                    scenarioGroupRepository_.scenario(
                                                      component.getScenarioGroupId()));
}
} // namespace Antares::Optimisation
