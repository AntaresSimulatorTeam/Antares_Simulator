// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once
#include "antares/expressions/IEvaluationContextProvider.h"
#include "antares/expressions/visitors/EvaluationContext.h"
#include "antares/study/system-model/component.h"

#include "scenarioGroupRepo.h"

namespace Antares::Optimisation
{
class EvaluationContextProvider final: public Expressions::IEvaluationContextProvider
{
public:
    explicit EvaluationContextProvider(const LinearProblemApi::ILinearProblemData& data,
                                       const ScenarioGroupRepository& scenarioGroupRepository,
                                       std::map<std::string, double> variables = {});
    ~EvaluationContextProvider() override = default;

    [[nodiscard]] Expressions::Visitors::EvaluationContext provide(
      const ModelerStudy::SystemModel::Component& component) const override;

private:
    const LinearProblemApi::ILinearProblemData& data_;
    const ScenarioGroupRepository& scenarioGroupRepository_;
    const std::map<std::string, double> variables_;
};
} // namespace Antares::Optimisation
