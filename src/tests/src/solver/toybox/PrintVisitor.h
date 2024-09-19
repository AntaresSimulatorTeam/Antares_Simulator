
/*
 * Copyright 2007-2024, RTE (https://www.rte-france.com)
 * See AUTHORS.txt
 * SPDX-License-Identifier: MPL-2.0
 * This file is part of Antares-Simulator,
 * Adequacy and Performance assessment for interconnected energy networks.
 *
 * Antares_Simulator is free software: you can redistribute it and/or modify
 * it under the terms of the Mozilla Public Licence 2.0 as published by
 * the Mozilla Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * Antares_Simulator is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * Mozilla Public Licence 2.0 for more details.
 *
 * You should have received a copy of the Mozilla Public Licence 2.0
 * along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
 */

#pragma once
#include <tree/ParseTreeVisitor.h>

#include "tree/ParseTree.h"

class PrintVisitor: public antlr4::tree::ParseTreeVisitor
{
public:
    virtual antlrcpp::Any visitChildren(antlr4::tree::ParseTree* node) override
    {
        for (auto child: node->children)
        {
            child->accept(this);
        }
        return antlrcpp::Any();
    }

    std::any visit(antlr4::tree::ParseTree* tree) override
    {
        std::cout << "visit " << tree->getText() << "\n";
        return std::any();
    }

    std::any visitTerminal(antlr4::tree::TerminalNode* node) override
    {
        std::cout << "visitTerminal " << node->getText() << "\n";
        return std::any();
    }

    std::any visitErrorNode(antlr4::tree::ErrorNode* node) override
    {
        return std::any();
    }
};
