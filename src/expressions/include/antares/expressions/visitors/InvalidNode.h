
// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once
#include <stdexcept>

namespace Antares::Expressions::Visitors
{
class InvalidNode: public std::invalid_argument
{
public:
    explicit InvalidNode(const std::string& node_name);
};

class NodeTypeShouldBeInExtraOutput: public std::invalid_argument
{
public:
    explicit NodeTypeShouldBeInExtraOutput(const std::string& node_name);
};

} // namespace Antares::Expressions::Visitors
