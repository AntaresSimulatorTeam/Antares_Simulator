// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <string>
#include <unordered_set>

#include "yaml-cpp/yaml.h"

namespace YAML
{

/// What to do when a map contains keys outside mandatoryFields + optionalFields.
enum class UnexpectedFields
{
    Warn,  ///< log a warning and ignore them
    Throw, ///< throw YAML::Exception
};

/// Throws YAML::Exception if \p node is a map lacking a mandatory key.
/// Unexpected keys (outside mandatoryFields + optionalFields) are handled according to
/// \p unexpectedPolicy.
/// Non-map nodes are ignored.
void checkFields(const Node& node,
                 const std::unordered_set<std::string>& mandatoryFields,
                 const std::unordered_set<std::string>& optionalFields = {},
                 UnexpectedFields unexpectedPolicy = UnexpectedFields::Warn);

} // namespace YAML
