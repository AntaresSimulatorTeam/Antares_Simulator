// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/io/inputs/yml-system/parser.h"

#include "antares/io/inputs/yml-system/system.h"

#include "decoders.hxx"

namespace Antares::IO::Inputs::YmlSystem
{

System Parser::parse(const std::string& content)
{
    YAML::Node root = YAML::Load(content);

    System system = root["system"].as<System>();

    return system;
}

} // namespace Antares::IO::Inputs::YmlSystem
