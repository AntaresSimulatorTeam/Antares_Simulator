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

#include "antares/study/system-model/component.h"

Antares::ModelerStudy::SystemModel::Model createModelWithParameters(
  std::vector<Antares::ModelerStudy::SystemModel::Parameter> params);

Antares::ModelerStudy::SystemModel::Model createModelWithParameters();

Antares::ModelerStudy::SystemModel::Model createModelWithoutParameters();

Antares::ModelerStudy::SystemModel::Component createComponent(
  Antares::ModelerStudy::SystemModel::Model& model,
  const std::string& id = "component",
  unsigned index = 0);

Antares::ModelerStudy::SystemModel::Component createComponent(
  Antares::ModelerStudy::SystemModel::Model& model,
  const std::string& id,
  std::map<std::string, Antares::ModelerStudy::SystemModel::ParameterTypeAndValue> parameter_values,
  unsigned index = 0);

std::pair<std::string, Antares::ModelerStudy::SystemModel::ParameterTypeAndValue>
build_context_parameter_with(
  const std::string& id,
  const std::string& value,
  const Antares::ModelerStudy::SystemModel::ParameterType& type = Antares::ModelerStudy::
    SystemModel::ParameterType::CONSTANT);
