// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/io/inputs/yml-utils/YmlTreeDisplayer.h"

#include <iterator>

namespace YAML
{

YmlTreeDisplayer::YmlTreeDisplayer(const Node& node):
    nodeTagPath_(node.Tag())
{
    for (const auto& entry: node)
    {
        const Node keyNode = entry.first;
        std::string keyName = keyNode.IsDefined() ? keyNode.as<std::string>()
                                                  : std::string("<unknown>");
        const auto mark = keyNode.Mark();
        const int line = static_cast<int>(mark.line) + 1;
        actualKeysLine_.emplace(keyName, line);
    }

    const auto depthParts = static_cast<std::size_t>(
      std::distance(nodeTagPath_.begin(), nodeTagPath_.end()));
    indentSpaces_ = (depthParts > 0 ? depthParts - 1 : 0) * 4;

    baseTree_ = Antares::IO::Inputs::YmlUtils::getBaseTree(nodeTagPath_);
}

const std::string& YmlTreeDisplayer::baseTree() const
{
    return baseTree_;
}

std::string YmlTreeDisplayer::buildMarkedTree(const std::vector<std::string>& unexpected,
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

} // namespace YAML
