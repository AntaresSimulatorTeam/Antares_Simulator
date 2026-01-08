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

#include <map>
#include <set>
#include <stdexcept>
#include <typeindex>

#include "antares/expressions/nodes/FunctionNode.h"
#include "antares/expressions/nodes/Node.h"

namespace Antares::IO::Inputs::ModelConverter
{

template<typename NodeType>
std::type_index typeIndexOf()
{
    static_assert(!std::is_same_v<NodeType, Expressions::Nodes::FunctionNode>,
                  "Use Expressions::Nodes::FunctionNodeType enum values or "
                  "typeIndexOf(Expressions::Nodes::FunctionNodeType) "
                  "instead of FunctionNode for forbidden rules.");
    return std::type_index(typeid(NodeType));
}

template<Expressions::Nodes::FunctionNodeType T>
std::type_index typeIndexOf()
{
    using Tag = std::integral_constant<Expressions::Nodes::FunctionNodeType, T>;
    return std::type_index(typeid(Tag));
}

class ForbiddenNodes
{
public:
    // ------------------------- GLOBAL -------------------------
    template<typename... NodeType>
    void addGlobalForbidden()
    {
        (global_.insert(typeIndexOf<NodeType>()), ...);
    }

    template<Expressions::Nodes::FunctionNodeType... NodeType>
    void addGlobalForbidden()
    {
        (global_.insert(typeIndexOf<NodeType>()), ...);
    }

    // ---------------------- PARENT -> CHILD --------------------
    template<Expressions::Nodes::FunctionNodeType Parent, typename Child>
    requires(!std::is_same_v<Child, Expressions::Nodes::FunctionNodeType>)
    void addForbiddenFor()
    {
        rules_[typeIndexOf<Parent>()].insert(typeIndexOf<Child>());
    }

    template<Expressions::Nodes::FunctionNodeType Parent,
             Expressions::Nodes::FunctionNodeType Child>
    void addForbiddenFor()
    {
        rules_[typeIndexOf<Parent>()].insert(typeIndexOf<Child>());
    }

    template<typename Parent, Expressions::Nodes::FunctionNodeType Child>
    void addForbiddenFor()
    {
        rules_[typeIndexOf<Parent>()].insert(typeIndexOf<Child>());
    }

    // ---------------------- COMPILE-TIME CHECK ----------------------
    template<typename Parent, Expressions::Nodes::FunctionNodeType Child>
    [[nodiscard]] bool isForbiddenFor() const
    {
        return check(typeIndexOf<Parent>(), typeIndexOf<Child>());
    }

    template<typename Parent, typename Child>
    [[nodiscard]] bool isForbiddenFor() const
    {
        return check(typeIndexOf<Parent>(), typeIndexOf<Child>());
    }

    // ---------------------- RUNTIME CHECK ----------------------
    template<Expressions::Nodes::FunctionNodeType Child>
    [[nodiscard]] bool isForbiddenFor(const std::type_index& parentTypeId) const
    {
        return check(parentTypeId, typeIndexOf<Child>());
    }

    template<typename Child>
    [[nodiscard]] bool isForbiddenFor(const std::type_index& parentTypeId) const
    {
        return check(parentTypeId, typeIndexOf<Child>());
    }

    // ---------------------- GLOBALLY FORBIDDEN ----------------------
    template<typename NodeType>
    [[nodiscard]] bool isGloballyForbidden() const
    {
        return global_.contains(typeIndexOf<NodeType>());
    }

    template<Expressions::Nodes::FunctionNodeType NodeType>
    [[nodiscard]] bool isGloballyForbidden() const
    {
        return global_.contains(typeIndexOf<NodeType>());
    }

private:
    std::set<std::type_index> global_;

    // Parent --> set of children 
    std::map<std::type_index, std::set<std::type_index>> rules_;

    [[nodiscard]] bool check(const std::type_index& parentTypeId,
                             const std::type_index& childTypeId) const
    {
        // global forbidden child ?
        if (global_.contains(childTypeId))
        {
            return true;
        }

        // parent-specific forbidden child ?
        const auto& it = rules_.find(parentTypeId);
        if (it == rules_.end())
        {
            return false;
        }

        return it->second.contains(childTypeId);
    }
};

} // namespace Antares::IO::Inputs::ModelConverter
