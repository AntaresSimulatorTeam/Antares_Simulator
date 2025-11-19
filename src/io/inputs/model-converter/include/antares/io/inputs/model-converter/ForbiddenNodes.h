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

namespace Antares::IO::Inputs::ModelConverter
{

/*
 * std::type_index(typeid(parentNode)) only encodes the C++ dynamic type, NOT the semantic subtype
 * encoded inside FunctionNode via FunctionNodeType
 * So:
 * FunctionNode(FunctionNodeType::dual, ...);
 * FunctionNode(FunctionNodeType::max, ...);
 * Both have the same dynamic type: typeid(node) == typeid(FunctionNode)
 * Therefore nodeSpecificForbiddenTypes[std::type_index(typeid(FunctionNode))] has one and only one
 * entry for all function nodes.
 * Meaning what is forbidden for dual also becomes forbidden for max, min, pow, etc.
 * the solution is to Wrap FunctionNode type into a fake C++ type
 * addForbiddenTypeFor<FunctionNodeTag<FunctionNodeType::dual>, ChildNodeType>();
 */
// Generic fallback: use C++ type
template<typename NodeType>
std::type_index forbiddenKey()
{
    if constexpr (std::is_same_v<NodeType, Expressions::Nodes::FunctionNode>)
    {
        static_assert(!std::is_same_v<NodeType, Expressions::Nodes::FunctionNode>,
                      "Use FunctionNodeType enum values or forbiddenKey(FunctionNodeType) "
                      "instead of FunctionNode for forbidden rules.");
    }
    return std::type_index(typeid(NodeType));
}

// Specialization for FunctionNodeType as non-type template parameter
template<Expressions::Nodes::FunctionNodeType T>
std::type_index forbiddenKey()

{
    // Each enum value gets a unique type through template instantiation
    // struct FunctionTypeTag
    // {
    //     static constexpr auto value = T;
    // };
    using FunctionTypeTag = std::integral_constant<Expressions::Nodes::FunctionNodeType, T>;
    return std::type_index(typeid(FunctionTypeTag));
}

// std::type_index forbiddenKey(const Expressions::Nodes::Node& node)
// {
//     if (const auto fn = dynamic_cast<const Expressions::Nodes::FunctionNode*>(&node))
//     {
//         return forbiddenKey(fn->type());
//     }
//
//     return std::type_index(typeid(node));
// }

class ForbiddenNodes
{
public:
    // -------------------- Global forbidden --------------------
    template<typename NodeType>
    void addForbiddenType()
    {
        globalForbidden_.insert(forbiddenKey<NodeType>());
    }

    template<Expressions::Nodes::FunctionNodeType NodeType>
    void addForbiddenType()
    {
        globalForbidden_.insert(forbiddenKey<NodeType>());
    }

    // void addForbiddenType(Expressions::Nodes::FunctionNodeType type)
    // {
    //     globalForbidden_.insert(forbiddenKey(type));
    // }

    template<typename... NodeTypes>
    void addForbiddenTypes()
    {
        (addForbiddenType<NodeTypes>(), ...);
    }

    template<Expressions::Nodes::FunctionNodeType... NodeTypes>
    void addForbiddenTypes()
    {
        (addForbiddenType<NodeTypes>(), ...);
    }

    template<typename NodeType>
    bool isForbidden() const
    {
        return globalForbidden_.contains(forbiddenKey<NodeType>());
    }

    template<Expressions::Nodes::FunctionNodeType NodeType>
    bool isForbidden() const
    {
        return globalForbidden_.contains(forbiddenKey<NodeType>());
    }

    // bool isForbidden(Expressions::Nodes::FunctionNodeType type) const
    // {
    //     return globalForbidden_.contains(forbiddenKey(type));
    // }

    // bool isForbidden(const Expressions::Nodes::Node& node) const
    // {
    //     return globalForbidden_.contains(forbiddenKey(node));
    // }

    // -------------------- Specific forbidden --------------------

    template<class Parent, class Child>
    void addForbiddenTypeFor()
    {
        rules_[forbiddenKey<Parent>()].insert(forbiddenKey<Child>());
    }

    template<Expressions::Nodes::FunctionNodeType Parent, class Child>
    void addForbiddenTypeFor()
    {
        rules_[forbiddenKey<Parent>()].insert(forbiddenKey<Child>());
    }

    template<typename Parent, typename Child>
    bool isForbiddenFor() const
    {
        auto it = rules_.find(forbiddenKey<Parent>());
        if (it == rules_.end())
        {
            return false;
        }
        return it->second.contains(forbiddenKey<Child>());
    }

    const std::unordered_set<std::type_index>& getGlobalForbiddenTypes() const
    {
        return globalForbidden_;
    }

    template<typename ParentNodeType>
    const std::unordered_set<std::type_index>* getForbiddenTypesFor() const
    {
        auto it = rules_.find(forbiddenKey<ParentNodeType>());
        if (it == rules_.end())
        {
            return nullptr;
        }
        return &it->second;
    }

    template<Expressions::Nodes::FunctionNodeType Parent>
    const std::unordered_set<std::type_index>* getForbiddenTypesFor(
      Expressions::Nodes::FunctionNodeType parentType) const
    {
        auto it = rules_.find(forbiddenKey<Parent>());
        if (it == rules_.end())
        {
            return nullptr;
        }
        return &it->second;
    }

    void clear()
    {
        globalForbidden_.clear();
        rules_.clear();
    }

    size_t globalSize() const
    {
        return globalForbidden_.size();
    }

    size_t specificSize() const
    {
        return rules_.size();
    }

private:
    std::unordered_set<std::type_index> globalForbidden_;

    std::unordered_map<std::type_index, std::unordered_set<std::type_index>> rules_;
};
} // namespace Antares::IO::Inputs::ModelConverter
