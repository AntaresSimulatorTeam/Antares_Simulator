// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <string>
#include <vector>

#include "yaml-cpp/yaml.h"

namespace Antares::IO::Inputs::YmlUtils
{

struct Field
{
    std::string name;
    bool required = false;
    std::vector<Field> children = {};
};

/// Validates maps and sequence entries recursively before decoding. Logs unknown fields and
/// throws InputError containing missing fields, null required IDs, and duplicated fields. Value
/// conversion is left to the decoders, so malformed values cannot hide field errors elsewhere in
/// the document.
/// The diagnostic path is explicit; validation does not modify the document or its YAML tags.
void checkFields(const YAML::Node& node,
                 const std::vector<Field>& fields,
                 const std::string& path = "");

} // namespace Antares::IO::Inputs::YmlUtils
