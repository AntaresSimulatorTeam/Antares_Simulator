// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/io/inputs/yml-system/parser.h"

#include "antares/io/inputs/yml-schema/schemaValidation.h"
#include "antares/io/inputs/yml-system/decoders.h"
#include "antares/io/inputs/yml-system/system.h"

namespace Antares::IO::Inputs::YmlSystem
{

System Parser::parse(const std::string& content, const std::string& pathFromRoot)
{
    YAML::Node root = YAML::Load(content);
    YmlSchema::validate(root, YmlSchema::SchemaKind::System, pathFromRoot);
    return root["system"].as<System>();
}

} // namespace Antares::IO::Inputs::YmlSystem
