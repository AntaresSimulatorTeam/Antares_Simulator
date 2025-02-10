/*
** Copyright 2007-2024, RTE (https://www.rte-france.com)
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
#include <string>

#include <antares/expressions/nodes/Node.h>

namespace Antares::Expressions::Nodes
{
/**
 * @brief Represents a component node in a syntax tree.
 */
class ComponentNode: public Node
{
public:
    /**
     * @brief Constructs a component node with the specified ID and name.
     *
     * @param component_id The component ID.
     * @param component_name The component name.
     */
    explicit ComponentNode(const std::string& component_id, const std::string& component_name);
    /**
     * @brief Retrieves the component ID.
     *
     * @return The component ID.
     */
    const std::string& getComponentId() const;

    /**
     * @brief Retrieves the component name.
     *
     * @return The component name.
     */
    const std::string& getComponentName() const;

    bool operator==(const ComponentNode& other) const = default;

private:
    std::string component_id_;
    std::string component_name_;
};

/**
 * @brief Represents a component variable node in a syntax tree.
 */
class ComponentVariableNode: public ComponentNode
{
public:
    using ComponentNode::ComponentNode;

    std::string name() const override
    {
        return "ComponentVariableNode";
    }
};

/**
 * @brief Represents a component parameter node in a syntax tree.
 */
class ComponentParameterNode: public ComponentNode
{
public:
    using ComponentNode::ComponentNode;

    std::string name() const override
    {
        return "ComponentParameterNode";
    }
};
} // namespace Antares::Expressions::Nodes
