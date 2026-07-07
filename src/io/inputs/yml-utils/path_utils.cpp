// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/io/inputs/yml-utils/path_utils.h"

#include <filesystem>
#include <string>

#include <antares/io/inputs/InputError.h>

namespace Antares::IO::Inputs::YmlUtils
{

std::string printPathTree(const std::filesystem::path& p)
{
    std::string treeStr;
    std::size_t depth = 0;
    for (const auto& part: p)
    {
        if (depth == 0)
        {
            treeStr += part.string();
            treeStr += '\n';
        }
        else
        {
            treeStr += std::string((depth - 1) * 4, ' ');
            treeStr += "|__ ";
            treeStr += part.string();
            treeStr += '\n';
        }
        ++depth;
    }
    return treeStr;
}

std::string getBaseTree(const std::filesystem::path& nodeTagPath)
{
    return printPathTree(nodeTagPath);
}

bool requireMap(const ::YAML::Node& node, const char* typeName)
{
    if (node.IsMap())
    {
        return true;
    }
    if (node.IsDefined() && !node.IsNull())
    {
        throw InputError(std::string("Expected a YAML mapping for '") + typeName + "'");
    }
    return false;
}

} // namespace Antares::IO::Inputs::YmlUtils
