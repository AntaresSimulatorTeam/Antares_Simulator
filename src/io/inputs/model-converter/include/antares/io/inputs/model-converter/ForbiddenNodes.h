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
                  "typeIndexOf<Expressions::Nodes::FunctionNodeType>()"
                  "instead of FunctionNode for forbidden rules.");
    return std::type_index(typeid(NodeType));
}

template<Expressions::Nodes::FunctionNodeType item>
std::type_index typeIndexOf()
{
    using enum_item_as_type = std::integral_constant<Expressions::Nodes::FunctionNodeType, item>;
    return std::type_index(typeid(enum_item_as_type));
}

class ForbiddenNodes
{
public:
    // ------ GLOBALLY FORBIDDEN -------
    template<typename... NodeType>
    void forbidGlobally()
    {
        (global_.insert(typeIndexOf<NodeType>()), ...);
    }

    template<Expressions::Nodes::FunctionNodeType... NodeType>
    void forbidGlobally()
    {
        (global_.insert(typeIndexOf<NodeType>()), ...);
    }

    // ------ A PARENT FORBIDS A CHILD ------
    template<Expressions::Nodes::FunctionNodeType Parent, typename Child>
    requires(!std::is_same_v<Child, Expressions::Nodes::FunctionNodeType>)
    void parentForbidsChild()
    {
        rules_[typeIndexOf<Parent>()].insert(typeIndexOf<Child>());
    }

    template<Expressions::Nodes::FunctionNodeType Parent,
             Expressions::Nodes::FunctionNodeType Child>
    void parentForbidsChild()
    {
        rules_[typeIndexOf<Parent>()].insert(typeIndexOf<Child>());
    }

    template<typename Parent, Expressions::Nodes::FunctionNodeType Child>
    void parentForbidsChild()
    {
        rules_[typeIndexOf<Parent>()].insert(typeIndexOf<Child>());
    }

    bool isGloballyForbidden(const std::type_index& typeId) const;
    bool isForbiddenByParent(const std::type_index& parentTypeId,
                             const std::type_index& nodeTypeId) const;

private:
    std::set<std::type_index> global_;
    std::map<std::type_index, std::set<std::type_index>> rules_; // Parent --> set of children
};

} // namespace Antares::IO::Inputs::ModelConverter
