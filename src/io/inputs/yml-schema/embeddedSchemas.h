// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <string_view>
#include <vector>

namespace Antares::IO::Inputs::YmlSchema
{
struct EmbeddedSchema
{
    std::string_view fileName; ///< e.g. "common.schema.yml"
    std::string_view content;  ///< YAML text
};

/// Schemas copied into the binary at configure time, see CMakeLists.txt.
const std::vector<EmbeddedSchema>& embeddedSchemas();
} // namespace Antares::IO::Inputs::YmlSchema
