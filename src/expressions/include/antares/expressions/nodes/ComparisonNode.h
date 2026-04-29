// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <antares/expressions/nodes/BinaryNode.h>

namespace Antares::Expressions::Nodes
{
/**
 * @brief Represents a comparison node in a syntax tree.
 */
class ComparisonNode: public BinaryNode
{
public:
    using BinaryNode::BinaryNode;
};
} // namespace Antares::Expressions::Nodes
