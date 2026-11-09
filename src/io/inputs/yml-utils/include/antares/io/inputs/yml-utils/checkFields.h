// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <initializer_list>
#include <string>
#include <vector>

#include "yaml-cpp/yaml.h"

namespace Antares::IO::Inputs::YmlUtils
{

/// Accepted YAML shape of a field that has children.
enum class Shape
{
    MapOrSequence, ///< A map, or a sequence of maps (the default)
    Map            ///< A single map only; sequences are rejected
};

struct ExpectedField
{
    std::string name;
    bool required = false;
    std::vector<ExpectedField> children = {};
    Shape shape = Shape::MapOrSequence;
};

/// Validates maps and sequence entries recursively before decoding. Logs unknown fields and
/// throws InputError containing missing fields, null required IDs, duplicated fields, and invalid
/// mappings. Fields with children accept a map or sequence of maps unless shape is Shape::Map.
/// Optional null collections are treated as absent, but explicit mappings cannot be null. Value
/// conversion is left to the decoders, so malformed values cannot hide field errors elsewhere in
/// the document.
/// The diagnostic path is explicit; validation does not modify the document or its YAML tags.
void checkFields(const YAML::Node& node, const ExpectedField& field, const std::string& path = "");

/// Overload for a node accepting multiple top-level fields (prebuilt list).
void checkFields(const YAML::Node& node,
                 const std::vector<ExpectedField>& fields,
                 const std::string& path = "");

} // namespace Antares::IO::Inputs::YmlUtils
