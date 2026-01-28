// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once
#include <string>

#include <antares/expressions/nodes/Leaf.h>

namespace Antares::Expressions::Nodes
{
/**
 * @brief Represents a port field node in a syntax tree.
 */
class PortFieldNode: public Node
{
public:
    /**
     * @brief Constructs a port field node with the specified port and field names.
     *
     * @param port_name The port name.
     * @param field_name The field name.
     */
    explicit PortFieldNode(const std::string& port_name, const std::string& field_name);

    /**
     * @brief Retrieves the port name.
     *
     * @return The port name.
     */
    const std::string& getPortName() const;

    /**
     * @brief Retrieves the field name.
     *
     * @return The field name.
     */
    const std::string& getFieldName() const;

    bool operator==(const PortFieldNode& other) const = default;

    std::string name() const override
    {
        return "PortFieldNode";
    }

private:
    std::string port_name_;
    std::string field_name_;
};
} // namespace Antares::Expressions::Nodes
