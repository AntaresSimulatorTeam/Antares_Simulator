// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once
#include <string>

#include <antares/expressions/nodes/ParentNode.h>

namespace Antares::Expressions::Nodes
{
/**
 * @brief Represents a port field node where the expression is a sum.
 *
 * Optionally holds a default expression (its only operand), used when the port is not connected.
 */
class PortFieldSumNode final: public ParentNode
{
public:
    /**
     * @brief Constructs a port field sum node with the specified port and field names.
     *
     * @param port_name The port name.
     * @param field_name The field name.
     * @param no_connection_expr Expression used instead of the sum when the port is not
     * connected, may be nullptr.
     */
    explicit PortFieldSumNode(const std::string& port_name,
                              const std::string& field_name,
                              Node* no_connection_expr = nullptr);

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

    /**
     * @brief Retrieves the expression used when the port is not connected.
     *
     * @return The default expression, or nullptr if none was given.
     */
    Node* noConnectionExpr() const;

    std::string name() const override
    {
        return "PortFieldSumNode";
    }

private:
    std::string port_name_;
    std::string field_name_;
};
} // namespace Antares::Expressions::Nodes
