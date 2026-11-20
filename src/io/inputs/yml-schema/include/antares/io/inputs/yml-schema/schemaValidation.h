// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <string>

#include "yaml-cpp/yaml.h"

namespace Antares::IO::Inputs::YmlSchema
{

/// The GEMS input files that have a schema.
enum class SchemaKind
{
    ModelLibrary,
    System
};

/// Validates a parsed YAML document against the embedded GEMS JSON Schema of the given kind
/// (see schemas/README.md). Throws InputError listing every violation found, each with the line
/// and column of the offending node and its path in the document. The document is not modified.
///
/// Plain YAML scalars are typed as in the YAML 1.2 core schema (`true`/`false`, `null`/`~`,
/// integers, floats); quoted scalars are always strings. Duplicated mapping keys are reported
/// as errors. `context` (typically the file path) is only used as a prefix in the error message.
void validate(const YAML::Node& root, SchemaKind kind, const std::string& context = "");

} // namespace Antares::IO::Inputs::YmlSchema
