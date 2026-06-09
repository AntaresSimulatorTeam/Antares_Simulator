// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/io/inputs/yml-optim-config/parser.h"

#include "antares/io/inputs/yml-optim-config/decoders.h"

namespace Antares::IO::Inputs::YmlOptimConfig
{
OptimConfig Parser::parse(const std::string& content)
{
    YAML::Node root = YAML::Load(content);

    // Parse the entire OptimConfig structure including resolution-mode
    auto optimConfig = root.as<OptimConfig>();

    return optimConfig;
}
} // namespace Antares::IO::Inputs::YmlOptimConfig
