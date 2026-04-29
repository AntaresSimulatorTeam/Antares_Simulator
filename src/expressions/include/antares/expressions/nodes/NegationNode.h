// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <antares/expressions/nodes/UnaryNode.h>

namespace Antares::Expressions::Nodes
{
/**
 * @brief Represents a negation node in a syntax tree.
 */
class NegationNode final: public UnaryNode
{
public:
    using UnaryNode::UnaryNode;

    std::string name() const override
    {
        return "NegationNode";
    }
};
} // namespace Antares::Expressions::Nodes
