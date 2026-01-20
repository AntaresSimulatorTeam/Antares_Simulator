// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once
#include <utility>

#include "antares/study/system-model/component.h"

Antares::ModelerStudy::SystemModel::Model createModelWithParameters(
  std::vector<Antares::ModelerStudy::SystemModel::Parameter> params);

Antares::ModelerStudy::SystemModel::Model createModelWithParameters();

Antares::ModelerStudy::SystemModel::Model createModelWithoutParameters();

Antares::ModelerStudy::SystemModel::Component createComponent(
  const Antares::ModelerStudy::SystemModel::Model& model,
  const std::string& id = "component",
  unsigned index = 0);

Antares::ModelerStudy::SystemModel::Component createComponent(
  const Antares::ModelerStudy::SystemModel::Model& model,
  const std::string& id,
  std::map<std::string, Antares::ModelerStudy::SystemModel::ParameterTypeAndValue> parameter_values,
  unsigned index = 0);

std::pair<std::string, Antares::ModelerStudy::SystemModel::ParameterTypeAndValue>
build_context_parameter_with(const std::string& id,
                             const std::string& value,
                             const Antares::Optimisation::VariabilityType& type = Antares::
                               Optimisation::VariabilityType::CONSTANT_IN_TIME_AND_SCENARIO);
