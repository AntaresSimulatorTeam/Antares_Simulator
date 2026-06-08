// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include <string>

#include "antares/modeler-optimisation-container/scenarioGroupRepo.h"

namespace Antares::Optimisation
{

ScenarioGroupRepository::AlreadyExists::AlreadyExists(const std::string& groupId):
    std::invalid_argument("Scenario group '" + groupId + "' already exists in group repo.")
{
}

ScenarioGroupRepository::DoesNotExist::DoesNotExist(const std::string& groupId):
    std::invalid_argument("Scenario group '" + groupId + "' does not exist in group repo.")
{
}

} // namespace Antares::Optimisation
