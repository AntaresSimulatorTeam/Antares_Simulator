// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/io/inputs/yml-system/parser.h"

#include "antares/io/inputs/yml-system/decoders.h"
#include "antares/io/inputs/yml-system/system.h"

namespace Antares::IO::Inputs::YmlSystem
{
namespace
{
void tagNodes(YAML::Node& node);

void visitMap(YAML::Node& node)
{
    for (auto it = node.begin(); it != node.end(); ++it)
    {
        YAML::Node child = it->second;
        child.SetTag(node.Tag() + "/" + it->first.as<std::string>());
        tagNodes(child);
    }
}

void visitSequence(YAML::Node& node)
{
    for (std::size_t i = 0; i < node.size(); ++i)
    {
        YAML::Node child = node[i];

        std::string childName = "[" + std::to_string(i) + "]";
        if (child.IsMap())
        {
            auto childIdNode = child["id"];
            if (childIdNode.IsDefined() && !childIdNode.IsNull())
            {
                childName = childIdNode.as<std::string>();
            }
        }
        child.SetTag(node.Tag() + "/" + childName);

        tagNodes(child);
    }
}

void tagNodes(YAML::Node& node)
{
    if (!node || node.IsNull())
    {
        return;
    }

    if (node.IsMap())
    {
        visitMap(node);
    }
    else if (node.IsSequence())
    {
        visitSequence(node);
    }
}
} // namespace

System Parser::parse(const std::string& content, const std::string& pathFromRoot)
{
    YAML::Node root = YAML::Load(content);

    auto systemNode = root["system"];
    if (systemNode.IsDefined() && !systemNode.IsNull())
    {
        systemNode.SetTag(pathFromRoot);
        tagNodes(systemNode);
    }

    System system = root["system"].as<System>();

    return system;
}

} // namespace Antares::IO::Inputs::YmlSystem
