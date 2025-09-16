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
#include <utility>
#include "antares/expressions/IEvaluationContextProvider.h"
#include "antares/study/system-model/component.h"

static Antares::ModelerStudy::SystemModel::Model createModelWithParameters()
{
    Antares::ModelerStudy::SystemModel::ModelBuilder model_builder;
    return model_builder.withId("model")
      .withParameters({Antares::ModelerStudy::SystemModel::Parameter(
                         "param1",
                         Antares::ModelerStudy::SystemModel::TimeDependent::NO,
                         Antares::ModelerStudy::SystemModel::ScenarioDependent::NO),
                       Antares::ModelerStudy::SystemModel::Parameter(
                         "param2",
                         Antares::ModelerStudy::SystemModel::TimeDependent::NO,
                         Antares::ModelerStudy::SystemModel::ScenarioDependent::NO)})
      .build();
}

static Antares::ModelerStudy::SystemModel::Model createModelWithoutParameters()
{
    Antares::ModelerStudy::SystemModel::ModelBuilder model_builder;
    return model_builder.withId("model").build();
}

static std::pair<std::string, Antares::Expressions::Visitors::ParameterTypeAndValue>
build_context_parameter_with(const std::string& id,
                             const std::string& value,
                             const Antares::Expressions::Visitors::ParameterType& type)
{
    return {id, {.id = id, .type = type, .value = value}};
}

