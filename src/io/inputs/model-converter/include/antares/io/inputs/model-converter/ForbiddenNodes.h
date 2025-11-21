/*
 * Copyright 2007-2025, RTE (https://www.rte-france.com)
 * See AUTHORS.txt
 * SPDX-License-Identifier: MPL-2.0
 * This file is part of Antares-Simulator,
 * Adequacy and Performance assessment for interconnected energy networks.
 *
 * Antares_Simulator is free software: you can redistribute it and/or modify
 * it under the terms of the Mozilla Public Licence 2.0 as published by
 * the Mozilla Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * Antares_Simulator is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * Mozilla Public Licence 2.0 for more details.
 *
 * You should have received a copy of the Mozilla Public Licence 2.0
 * along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
 */

#pragma once

#include <stdexcept>
#include <typeindex>
#include <unordered_map>
#include <unordered_set>

#include "antares/expressions/nodes/FunctionNode.h"
#include "antares/expressions/nodes/Node.h"
using namespace Antares::Expressions::Nodes;

namespace Antares::IO::Inputs::ModelConverter
{
template<typename NodeType>
std::type_index forbiddenNodeKey()
{
    if constexpr (std::is_same_v<NodeType, Expressions::Nodes::FunctionNode>)
    {
        static_assert(!std::is_same_v<NodeType, Expressions::Nodes::FunctionNode>,
                      "Use FunctionNodeType enum values or forbiddenNodeKey(FunctionNodeType) "
                      "instead of FunctionNode for forbidden rules.");
    }
    return std::type_index(typeid(NodeType));
}

template<FunctionNodeType T>
inline std::type_index forbiddenNodeKey()
{
    using Tag = std::integral_constant<FunctionNodeType, T>;
    return std::type_index(typeid(Tag));
}

inline std::type_index forbiddenNodeKey(const FunctionNodeType& funcType)
{
    switch (funcType)
    {
    case FunctionNodeType::max:
    {
        using Tag = std::integral_constant<FunctionNodeType, FunctionNodeType::max>;
        return std::type_index(typeid(Tag));
    }
    case FunctionNodeType::min:
    {
        using Tag = std::integral_constant<FunctionNodeType, FunctionNodeType::min>;
        return std::type_index(typeid(Tag));
    }
    case FunctionNodeType::pow:
    {
        using Tag = std::integral_constant<FunctionNodeType, FunctionNodeType::pow>;
        return std::type_index(typeid(Tag));
    }
    case FunctionNodeType::dual:
    {
        using Tag = std::integral_constant<FunctionNodeType, FunctionNodeType::dual>;
        return std::type_index(typeid(Tag));
    }
    case FunctionNodeType::reduced_cost:
    {
        using Tag = std::integral_constant<FunctionNodeType, FunctionNodeType::reduced_cost>;
        return std::type_index(typeid(Tag));
    }
    default:
        throw std::runtime_error("ForbiddenNodeKey is not implemented");
    }
}

inline std::type_index forbiddenNodeKey(const Node& node)
{
    if (auto* funcNode = dynamic_cast<const FunctionNode*>(&node))
    {
        return forbiddenNodeKey(funcNode->type());
    }
    return std::type_index(typeid(node));
}

class ForbiddenNodes
{
public:
    // ------------------------- GLOBAL -------------------------

    template<typename... NodeType>
    void addGlobalForbidden()
    {
        (global_.insert(forbiddenNodeKey<NodeType>()), ...);
    }

    template<FunctionNodeType... NodeType>
    void addGlobalForbidden()
    {
        (global_.insert(forbiddenNodeKey<NodeType>()), ...);
    }

    // ---------------------- PARENT -> CHILD --------------------
    template<FunctionNodeType Parent, typename Child>
    requires(!std::is_same_v<Child, FunctionNodeType>)
    void addForbiddenFor()
    {
        rules_[forbiddenNodeKey<Parent>()].insert(forbiddenNodeKey<Child>());
    }

    template<FunctionNodeType Parent, FunctionNodeType Child>
    void addForbiddenFor()
    {
        rules_[forbiddenNodeKey<Parent>()].insert(forbiddenNodeKey<Child>());
    }

    template<typename Parent, FunctionNodeType Child>
    void addForbiddenFor()
    {
        rules_[forbiddenNodeKey<Parent>()].insert(forbiddenNodeKey<Child>());
    }

    bool check(const std::type_index& parentKey, const std::type_index& childKey) const
    {
        // global forbidden child?
        if (global_.contains(childKey))
        {
            return true;
        }

        // parent-specific forbidden child?
        auto it = rules_.find(parentKey);
        if (it == rules_.end())
        {
            return false;
        }

        return it->second.contains(childKey);
    }

    // ---------------------- COMPILE-TIME CHECK ----------------------
    template<typename Parent, FunctionNodeType Child>
    bool isForbiddenFor() const
    {
        return check(forbiddenNodeKey<Parent>(), forbiddenNodeKey<Child>());
    }

    template<typename Parent, typename Child>
    bool isForbiddenFor() const
    {
        return check(forbiddenNodeKey<Parent>(), forbiddenNodeKey<Child>());
    }

    // ---------------------- RUNTIME CHECK ----------------------
    template<FunctionNodeType Parent>
    bool isForbiddenFor(const Node& child) const
    {
        return check(forbiddenNodeKey<Parent>(), forbiddenNodeKey(child));
    }

    template<FunctionNodeType Child>
    bool isForbiddenFor(const std::type_index& parentKey) const
    {
        return check(parentKey, forbiddenNodeKey<Child>());
    }

    template<typename Child>
    bool isForbiddenFor(const std::type_index& parentKey) const
    {
        return check(parentKey, forbiddenNodeKey<Child>());
    }

    bool isForbiddenFor(const std::type_index& parentKey, const Node& child) const
    {
        return check(parentKey, forbiddenNodeKey(child));
    }

    template<typename NodeType>
    bool isForbidden() const
    {
        return global_.contains(forbiddenNodeKey<NodeType>());
    }

    template<FunctionNodeType NodeType>
    bool isForbidden() const
    {
        return global_.contains(forbiddenNodeKey<NodeType>());
    }
private:
    std::unordered_set<std::type_index> global_;
    std::unordered_map<std::type_index, std::unordered_set<std::type_index>> rules_;
};

} // namespace Antares::IO::Inputs::ModelConverter