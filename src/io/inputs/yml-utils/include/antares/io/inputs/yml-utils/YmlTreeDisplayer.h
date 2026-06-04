// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <filesystem>
#include <string>
#include <unordered_map>
#include <vector>

#include "antares/io/inputs/yml-utils/path_utils.h"

#include "yaml-cpp/yaml.h"

namespace YAML
{

/// Builds a textual tree representation of a YAML map node,
/// annotating unexpected and missing fields for error reporting.
class YmlTreeDisplayer
{
public:
    explicit YmlTreeDisplayer(const Node& node);

    const std::string& baseTree() const;

    std::string buildMarkedTree(const std::vector<std::string>& unexpected,
                                const std::vector<std::string>& missing) const;

private:
    std::filesystem::path nodeTagPath_;
    std::unordered_map<std::string, int> actualKeysLine_;
    std::size_t indentSpaces_ = 0;
    std::string baseTree_;
};

} // namespace YAML
