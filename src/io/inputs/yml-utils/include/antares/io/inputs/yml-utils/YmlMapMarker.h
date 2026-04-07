// Copyright 2007-2026, RTE
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <filesystem>
#include <iterator>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "antares/io/inputs/yml-utils/path_utils.h"

#include "yaml-cpp/yaml.h"

namespace YAML
{

class YmlMapMarker
{
public:
    explicit YmlMapMarker(const Node& node):
        node_(node),
        nodeTagPath_(node.Tag())
    {
        for (const auto& entry: node_)
        {
            const Node keyNode = entry.first;
            std::string keyName = keyNode.IsDefined() ? keyNode.as<std::string>()
                                                      : std::string("<unknown>");
            const auto mark = keyNode.Mark();
            const int line = static_cast<int>(mark.line) + 1;
            actualKeysLine_.emplace(keyName, line);
            actualSet_.insert(keyName);
        }

        depthParts_ = static_cast<std::size_t>(
          std::distance(nodeTagPath_.begin(), nodeTagPath_.end()));
        if (depthParts_ == 0)
        {
            depthParts_ = 1;
        }
        indentSpaces_ = (depthParts_ - 1) * 4;

        baseTree_ = Antares::IO::Inputs::YmlUtils::getBaseTree(nodeTagPath_);
    }

    const std::unordered_map<std::string, int>& actualKeysLine() const
    {
        return actualKeysLine_;
    }

    const std::unordered_set<std::string>& actualSet() const
    {
        return actualSet_;
    }

    const std::string& baseTree() const
    {
        return baseTree_;
    }

    std::size_t indentSpaces() const
    {
        return indentSpaces_;
    }

    std::string buildMarkedTreeForUnexpectedAndMissing(
      const std::vector<std::string>& unexpected,
      const std::vector<std::string>& missing) const
    {
        std::string markedFieldsTree;
        for (const auto& keyName: unexpected)
        {
            const int line = actualKeysLine_.count(keyName) ? actualKeysLine_.at(keyName) : -1;
            markedFieldsTree += std::string(indentSpaces_, ' ');
            markedFieldsTree += "|__X ";
            markedFieldsTree += keyName;
            if (line > 0)
            {
                markedFieldsTree += " at line ";
                markedFieldsTree += std::to_string(line);
            }
            markedFieldsTree += '\n';
        }
        for (const auto& keyName: missing)
        {
            markedFieldsTree += std::string(indentSpaces_, ' ');
            markedFieldsTree += "|__? ";
            markedFieldsTree += keyName;
            markedFieldsTree += " (missing)\n";
        }
        return markedFieldsTree;
    }

    std::string buildMarkedTreeAllPresent() const
    {
        std::string markedFieldsTree;
        for (const auto& [keyName, lineNr]: actualKeysLine_)
        {
            markedFieldsTree += std::string(indentSpaces_, ' ');
            markedFieldsTree += "|__X ";
            markedFieldsTree += keyName;
            markedFieldsTree += " at line ";
            markedFieldsTree += std::to_string(lineNr);
            markedFieldsTree += '\n';
        }
        return markedFieldsTree;
    }

private:
    const Node& node_;
    std::filesystem::path nodeTagPath_;
    std::unordered_map<std::string, int> actualKeysLine_;
    std::unordered_set<std::string> actualSet_;
    std::size_t depthParts_ = 1;
    std::size_t indentSpaces_ = 0;
    std::string baseTree_;
};

} // namespace YAML
