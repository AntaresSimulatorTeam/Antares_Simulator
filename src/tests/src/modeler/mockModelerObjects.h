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
#include <utility>

#include "antares/expressions/IEvaluationContextProvider.h"
#include "antares/study/system-model/component.h"

class MockEvaluationContextProvider: public Antares::Expressions::IEvaluationContextProvider
{
private:
    Antares::Expressions::Visitors::EvaluationContext context_;

public:
    MockEvaluationContextProvider(Antares::Expressions::Visitors::EvaluationContext context):
        context_(std::move(context))
    {
    }

    Antares::Expressions::Visitors::EvaluationContext provide(
      [[maybe_unused]] const Antares::ModelerStudy::SystemModel::Component& component) const
    {
        return context_;
    }
};

Antares::ModelerStudy::SystemModel::Model createModelWithParameters();
Antares::ModelerStudy::SystemModel::Model createModelWithoutParameters();
Antares::ModelerStudy::SystemModel::Component createComponent(const std::string& id = "component");
Antares::ModelerStudy::SystemModel::Component createComponent(
  const std::string& id,
  std::map<std::string, Antares::Expressions::Visitors::ParameterTypeAndValue> parameter_values);

std::pair<std::string, Antares::Expressions::Visitors::ParameterTypeAndValue>
build_context_parameter_with(const std::string& id,
                             const std::string& value,
                             const Antares::Expressions::Visitors::ParameterType& type = Antares::
                               Expressions::Visitors::ParameterType::CONSTANT);
