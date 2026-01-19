// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/io/inputs/yml-optim-config/parser.h"

#include "decoders.hxx"

namespace Antares::IO::Inputs::YmlOptimConfig
{
OptimConfig Parser::parse(const std::string& content)
{
    YAML::Node root = YAML::Load(content);

    OptimConfig optimConfig = root["models"].as<std::vector<Model>>();

    return optimConfig;
}
} // namespace Antares::IO::Inputs::YmlOptimConfig
