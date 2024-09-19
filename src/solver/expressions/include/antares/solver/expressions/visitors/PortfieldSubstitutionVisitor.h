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

#include "antares/solver/expressions/visitors/CloneVisitor.h"

namespace Antares::Solver::Visitors
{

struct KeyHasher
{
    std::size_t operator()(const Nodes::PortFieldNode& n) const;
};

/**
 * @brief Represents the context for performing substitutions in a syntax tree.
 */
struct PortfieldSubstitutionContext
{
    std::unordered_map<Nodes::PortFieldNode, Nodes::Node*, KeyHasher> portfield;
};

/**
 * @brief Represents a visitor for substituting portfield nodes in a syntax tree.
 */
class PortfieldSubstitutionVisitor: public CloneVisitor
{
public:
    PortfieldSubstitutionVisitor(Registry<Nodes::Node>& registry,
                                 PortfieldSubstitutionContext& ctx);

    PortfieldSubstitutionContext& ctx_;
    std::string name() const override;

private:
    // Only override visit method for PortField, clone the rest
    Nodes::Node* visit(const Nodes::PortFieldNode* node) override;
};
} // namespace Antares::Solver::Visitors
