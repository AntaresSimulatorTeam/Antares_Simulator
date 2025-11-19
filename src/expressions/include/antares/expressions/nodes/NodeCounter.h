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
#pragma once
#include <map>
#include <memory>
#include <typeindex>
#include <unordered_set>

#include "Node.h"

namespace Antares::Expressions::Nodes
{
struct NodeInfo
{
    unsigned int count;
    std::type_index type;
};

class NodeCounter
{
public:
    NodeCounter() = default;
    NodeCounter(const std::vector<std::unique_ptr<Nodes::Node>>& nodes);
    void computeNumberNodesPerType();

    void insertNode(const Nodes::Node* node);
    /**
     * @brief Retrieves a unique ID for a given node.
     *
     * Generates or retrieves a unique identifier for the specified node.
     *
     * @param node The node for which to get the ID.
     * @return An integer representing the unique ID of the node.
     */
    unsigned int getNodeID(const Nodes::Node* node);

    void reset();
    bool empty() const;

    const std::map<std::string, NodeInfo>& nbNodesPerType() const
    {
        return nbNodesPerType_;
    }

    unsigned int nodeCount() const
    {
        return nodeCount_;
    }

    bool contains(const std::string& nodeName) const;

    template<typename NodeType>
    bool contains() const
    {
        return std::ranges::find_if(nbNodesPerType_,
                                    [&](const std::pair<std::string, NodeInfo>& nodeInfos)
                                    {
                                        return nodeInfos.second.type
                                                 == std::type_index(typeid(NodeType))
                                               && nodeInfos.second.count != 0;
                                    })
               != nbNodesPerType_.end();
    }

private:
    /**
     * @brief A map of nodes to their unique IDs.
     *
     * This map is used to keep track of assigned IDs for each node in the AST.
     */
    std::map<const Nodes::Node*, unsigned int> nodeIds_;

    /**
     * @brief A map associating a number of instances to a type name.
     *
     * This map is used to keep track of assigned IDs for each node in the AST.
     */

    std::map<std::string, NodeInfo> nbNodesPerType_;
    /**
     * @brief Counter for generating unique node IDs.
     *
     * This counter is incremented each time a new node ID is needed.
     */
    unsigned int nodeCount_ = 0;

};
} // namespace Antares::Expressions::Nodes
