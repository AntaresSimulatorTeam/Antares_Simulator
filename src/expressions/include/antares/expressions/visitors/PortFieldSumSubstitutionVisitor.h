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

#include <map>

#include "antares/expressions/visitors/CloneVisitor.h"

namespace Antares::Expressions::Visitors
{

/**
 * @brief Represents the context for performing substitutions in a syntax tree.
 */
struct PortFieldSumSubstitutionContext
{
    std::unordered_map<Nodes::PortFieldSumNode, std::vector<Nodes::Node*>, PortFieldHash>
      portfieldSum;
};

/**
 * @brief Represents a visitor for substituting portfield sum nodes in a syntax tree.
 */
class PortFieldSumSubstitutionVisitor: public CloneVisitor
{
public:
    PortFieldSumSubstitutionVisitor(Registry<Nodes::Node>& registry,
                                    PortFieldSumSubstitutionContext& ctx);

    std::string name() const override;

private:
    // Only override visit method for PortFieldSum, clone the rest
    Nodes::Node* visit(const Nodes::PortFieldSumNode* node) override;

    Registry<Nodes::Node>& registry_;
    PortFieldSumSubstitutionContext& ctx_;
};
} // namespace Antares::Expressions::Visitors
