// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/io/inputs/yml-model/parser.h"

#include "antares/io/inputs/yml-model/Library.h"
#include "antares/io/inputs/yml-model/decoders.h"

namespace Antares::IO::Inputs::YmlModel
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

        auto childIdNode = child["id"];
        std::string childName = "__without__id__";
        if (childIdNode.IsDefined() && !childIdNode.IsNull())
        {
            childName = childIdNode.as<std::string>();
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
} // anonymous namespace

LibraryIdNotDefined::LibraryIdNotDefined():
    std::runtime_error("Library id is not defined")
{
}

Library Parser::parse(const std::string& content)
{
    YAML::Node root = YAML::Load(content);
    auto libraryNode = root["library"];
    if (libraryNode.IsDefined() && !libraryNode.IsNull())
    {
        auto libraryId = libraryNode["id"];
        if (libraryId.IsDefined() && !libraryId.IsNull())
        {
            auto libraryName = libraryId.as<std::string>();
            libraryNode.SetTag(libraryName);
            tagNodes(libraryNode);
        }
        else
        {
            throw LibraryIdNotDefined();
        }
    }
    auto library = root["library"].as<Library>();

    return library;
}
} // namespace Antares::IO::Inputs::YmlModel
