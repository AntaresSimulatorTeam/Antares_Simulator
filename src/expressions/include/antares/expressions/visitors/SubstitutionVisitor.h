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

#include <unordered_set>

#include <antares/expressions/Registry.hxx>
#include <antares/expressions/nodes/NodesForwardDeclaration.h>
#include <antares/expressions/visitors/CloneVisitor.h>

namespace Antares::Expressions::Visitors
{
/**
 * @brief Represents the context for performing substitutions in a syntax tree.
 */
struct SubstitutionContext
{
    std::unordered_set<Nodes::ComponentVariableNode*> variables;
};

/**
 * @brief Represents a visitor for substituting component variables in a syntax tree.
 *
 * @param registry The registry used for creating new nodes.
 * @param ctx The substitution context.
 */
class SubstitutionVisitor: public CloneVisitor
{
public:
    SubstitutionVisitor(Registry<Nodes::Node>& registry, SubstitutionContext& ctx);

    SubstitutionContext& ctx_;
    Registry<Nodes::Node>& registry_;
    std::string name() const override;

private:
    // Only override visit method for ComponentVariableNode, clone the rest
    Nodes::Node* visit(const Nodes::ComponentVariableNode* node) override;
};
} // namespace Antares::Expressions::Visitors
