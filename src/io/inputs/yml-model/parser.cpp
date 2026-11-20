// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/io/inputs/yml-model/parser.h"

#include "antares/io/inputs/yml-model/Library.h"
#include "antares/io/inputs/yml-model/decoders.h"
#include "antares/io/inputs/yml-schema/schemaValidation.h"

namespace Antares::IO::Inputs::YmlModel
{

Library Parser::parse(const std::string& content)
{
    YAML::Node root = YAML::Load(content);
    YmlSchema::validate(root, YmlSchema::SchemaKind::ModelLibrary);
    return root["library"].as<Library>();
}
} // namespace Antares::IO::Inputs::YmlModel
