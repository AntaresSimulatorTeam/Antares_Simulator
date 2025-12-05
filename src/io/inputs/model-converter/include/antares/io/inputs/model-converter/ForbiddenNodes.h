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

template<typename NodeType>
std::type_index forbiddenNodeKey()
{
    static_assert(!std::is_same_v<NodeType, Expressions::Nodes::FunctionNode>,
                  "Use Expressions::Nodes::FunctionNodeType enum values or "
                  "forbiddenNodeKey(Expressions::Nodes::FunctionNodeType) "
                  "instead of FunctionNode for forbidden rules.");
    return std::type_index(typeid(NodeType));
}

template<Expressions::Nodes::FunctionNodeType T>
std::type_index forbiddenNodeKey()
{
    using Tag = std::integral_constant<Expressions::Nodes::FunctionNodeType, T>;
    return std::type_index(typeid(Tag));
}

inline std::type_index forbiddenNodeKey(const Expressions::Nodes::FunctionNodeType& funcType)
{
    switch (funcType)
    {
        using NT = Expressions::Nodes::FunctionNodeType;
    case NT::max:
        return forbiddenNodeKey<NT::max>();
    case NT::min:
        return forbiddenNodeKey<NT::min>();
    case NT::pow:
        return forbiddenNodeKey<NT::pow>();
    case NT::dual:
        return forbiddenNodeKey<NT::dual>();
    case NT::reduced_cost:
        return forbiddenNodeKey<NT::reduced_cost>();
    default:
        throw std::runtime_error("ForbiddenNodeKey is not implemented");
    }
}

inline std::type_index forbiddenNodeKey(const Expressions::Nodes::Node& node)
{
    if (auto* funcNode = dynamic_cast<const Expressions::Nodes::FunctionNode*>(&node))
    {
        return forbiddenNodeKey(funcNode->type());
    }
    return {typeid(node)};
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

    template<Expressions::Nodes::FunctionNodeType... NodeType>
    void addGlobalForbidden()
    {
        (global_.insert(forbiddenNodeKey<NodeType>()), ...);
    }

    // ---------------------- PARENT -> CHILD --------------------
    template<Expressions::Nodes::FunctionNodeType Parent, typename Child>
    requires(!std::is_same_v<Child, Expressions::Nodes::FunctionNodeType>)
    void addForbiddenFor()
    {
        rules_[forbiddenNodeKey<Parent>()].insert(forbiddenNodeKey<Child>());
    }

    template<Expressions::Nodes::FunctionNodeType Parent,
             Expressions::Nodes::FunctionNodeType Child>
    void addForbiddenFor()
    {
        rules_[forbiddenNodeKey<Parent>()].insert(forbiddenNodeKey<Child>());
    }

    template<typename Parent, Expressions::Nodes::FunctionNodeType Child>
    void addForbiddenFor()
    {
        rules_[forbiddenNodeKey<Parent>()].insert(forbiddenNodeKey<Child>());
    }

    // ---------------------- COMPILE-TIME CHECK ----------------------
    template<typename Parent, Expressions::Nodes::FunctionNodeType Child>
    [[nodiscard]] bool isForbiddenFor() const
    {
        return check(forbiddenNodeKey<Parent>(), forbiddenNodeKey<Child>());
    }

    template<typename Parent, typename Child>
    [[nodiscard]] bool isForbiddenFor() const
    {
        return check(forbiddenNodeKey<Parent>(), forbiddenNodeKey<Child>());
    }

    // ---------------------- RUNTIME CHECK ----------------------

    template<Expressions::Nodes::FunctionNodeType Child>
    [[nodiscard]] bool isForbiddenFor(const std::type_index& parentKey) const
    {
        return check(parentKey, forbiddenNodeKey<Child>());
    }

    template<typename Child>
    [[nodiscard]] bool isForbiddenFor(const std::type_index& parentKey) const
    {
        return check(parentKey, forbiddenNodeKey<Child>());
    }

    // ---------------------- GLOBALLY FORBIDDEN ----------------------

    template<typename NodeType>
    [[nodiscard]] bool isForbidden() const
    {
        return global_.contains(forbiddenNodeKey<NodeType>());
    }

    template<Expressions::Nodes::FunctionNodeType NodeType>
    [[nodiscard]] bool isForbidden() const
    {
        return global_.contains(forbiddenNodeKey<NodeType>());
    }

private:
    std::unordered_set<std::type_index> global_;
    std::unordered_map<std::type_index /* parent */,
                       std::unordered_set<std::type_index> /* children */>
      rules_;

    [[nodiscard]] bool check(const std::type_index& parentKey,
                             const std::type_index& childKey) const
    {
        // global forbidden child?
        if (global_.contains(childKey))
        {
            return true;
        }

        // parent-specific forbidden child?
        const auto& it = rules_.find(parentKey);
        if (it == rules_.end())
        {
            return false;
        }

        return it->second.contains(childKey);
    }
};

} // namespace Antares::IO::Inputs::ModelConverter
