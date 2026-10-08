// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <string>
#include <unordered_set>

#include "yaml-cpp/yaml.h"

namespace YAML
{

/// Throws YAML::Exception if \p node is a map with keys outside
/// mandatoryFields + optionalFields, or lacking a mandatory key.
/// Non-map nodes are ignored.
void checkFields(const Node& node,
                 const std::unordered_set<std::string>& mandatoryFields,
                 const std::unordered_set<std::string>& optionalFields = {});

} // namespace YAML
