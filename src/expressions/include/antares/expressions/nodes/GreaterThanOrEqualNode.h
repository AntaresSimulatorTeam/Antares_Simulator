// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <antares/expressions/nodes/ComparisonNode.h>

namespace Antares::Expressions::Nodes
{
/**
 * @brief Represents a greater than or equal comparison node in a syntax tree.
 */
class GreaterThanOrEqualNode final: public ComparisonNode
{
public:
    using ComparisonNode::ComparisonNode;

    std::string name() const override
    {
        return "GreaterThanOrEqualNode";
    }
};
} // namespace Antares::Expressions::Nodes
