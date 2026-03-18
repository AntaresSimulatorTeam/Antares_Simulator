// Copyright 2007-2026, RTE
// SPDX-License-Identifier: MPL-2.0

#include <filesystem>
#include <iterator>
#include <string>

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

} // namespace Antares::IO::Inputs::YmlUtils
