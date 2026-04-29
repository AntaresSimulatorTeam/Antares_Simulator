// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <antares/expressions/nodes/ComparisonNode.h>

namespace Antares::Expressions::Nodes
{
/**
 * @brief Represents an equality comparison node in a syntax tree.
 */
class EqualNode final: public ComparisonNode
{
public:
    using ComparisonNode::ComparisonNode;

    std::string name() const override
    {
        return "EqualNode";
    }
};
} // namespace Antares::Expressions::Nodes
