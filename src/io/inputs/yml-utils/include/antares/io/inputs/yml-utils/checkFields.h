// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <string>
#include <unordered_set>

#include <antares/io/inputs/InputError.h>

#include "yaml-cpp/yaml.h"

namespace YAML
{

/// Thrown by checkFields, outside of a DiagnosticsScope, when the only problem is keys outside
/// mandatoryFields + optionalFields.
class UnexpectedFieldsError: public Antares::IO::Inputs::InputError
{
public:
    using Antares::IO::Inputs::InputError::InputError;
};

/// Checks that \p node, if it is a map, has all of mandatoryFields, no duplicated key, and no key
/// outside mandatoryFields + optionalFields. Absent optional fields are not an error. Non-map nodes
/// are ignored.
///
/// With an active YmlUtils::DiagnosticsScope, problems are recorded in it (missing and duplicated
/// fields as errors, unexpected fields as warnings) and the function returns false if \p node
/// cannot be decoded reliably (missing or duplicated field), true otherwise.
/// Without one, it throws InputError (missing or duplicated field) or UnexpectedFieldsError.
bool checkFields(const Node& node,
                 const std::unordered_set<std::string>& mandatoryFields,
                 const std::unordered_set<std::string>& optionalFields = {});

} // namespace YAML
