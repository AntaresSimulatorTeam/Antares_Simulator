/*
** Copyright 2007-2025, RTE (https://www.rte-france.com)
** See AUTHORS.txt
** SPDX-License-Identifier: MPL-2.0
** This file is part of Antares-Simulator,
** Adequacy and Performance assessment for interconnected energy networks.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the Mozilla Public Licence 2.0 as published by
** the Mozilla Foundation, either version 2 of the License, or
** (at your option) any later version.
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/
#include "antares/expressions/nodes/NodeCounter.h"

#include <ranges>
#include <stdexcept>

namespace Antares::Expressions::Nodes
{
void NodeCounter::insertNode(const Nodes::Node* node)
{
    if (!node)
    {
        throw std::invalid_argument("cannot get node id from nullptr");
    }
    if (!nodeIds_.contains(node))
    {
        nodeIds_[node] = ++nodeCount_;
    }
}

unsigned int NodeCounter::getNodeID(const Nodes::Node* node)
{
    insertNode(node);
    return nodeIds_[node];
}

void NodeCounter::reset()
{
    nodeCount_ = 0;
    nodeIds_.clear();
    nbNodesPerType_.clear();
}

bool NodeCounter::empty() const
{
    return nbNodesPerType_.empty();
}

bool NodeCounter::contains(const std::string& nodeName) const
{
    return std::ranges::find_if(nbNodesPerType_,
                                [&nodeName](const std::pair<std::string, NodeInfo>& value)
                                { return value.first == nodeName && value.second.count != 0; })
           != nbNodesPerType_.end();
}

NodeCounter::NodeCounter(const std::vector<std::unique_ptr<Nodes::Node>>& nodes)
{
    for (const auto& node: nodes)
    {
        insertNode(node.get());
    }
}

void NodeCounter::computeNumberNodesPerType()
{
    nbNodesPerType_.clear();
    for (const auto& [node, _]: nodeIds_)
    {
        auto it = std::ranges::find_if(nbNodesPerType_,
                                       [&node](const std::pair<std::string, NodeInfo>& nodeInfos)
                                       { return nodeInfos.first == node->name(); });
        if (it == nbNodesPerType_.end())
        {
            nbNodesPerType_.try_emplace(node->name(),
                                        NodeInfo{.count = 1,
                                                 .type = std::type_index(typeid(node))});
            continue;
        }
        ++(it->second.count);
    }
}

} // namespace Antares::Expressions::Nodes
