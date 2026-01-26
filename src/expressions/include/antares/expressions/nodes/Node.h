// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once
#include <antares/expressions/IName.h>

namespace Antares::Expressions::Nodes
{
/**
 * @brief Base class for nodes in a syntax tree.
 */
class Node: public IName
{
public:
    virtual ~Node() = default;
};
} // namespace Antares::Expressions::Nodes
