// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <filesystem>
#include <string>

#include "yaml-cpp/yaml.h"

namespace Antares::IO::Inputs::YmlUtils
{

std::string printPathTree(const std::filesystem::path& p);

std::string getBaseTree(const std::filesystem::path& nodeTagPath);

bool requireMap(const YAML::Node& node, const char* typeName);

} // namespace Antares::IO::Inputs::YmlUtils
