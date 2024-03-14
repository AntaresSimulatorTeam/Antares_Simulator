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
#define BOOST_TEST_MODULE antlr_interface tests
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <boost/test/data/test_case.hpp>
#include "antlr4-runtime.h"
#include <iostream>

#include "ExprLexer.h"
#include "ExprParser.h"

using namespace antlr4;
BOOST_AUTO_TEST_CASE(test_antlr_interface)
{
    const std::string my_input = "y = b + ax";
    ANTLRInputStream input(my_input);
    ExprLexer lexer(&input);
    CommonTokenStream tokens(&lexer);
    tokens.fill();
    for (auto token : tokens.getTokens())
    {
        std::cout << token->toString() << std::endl;
    }
    BOOST_CHECK(tokens.getTokens().size() == 6);
    // ExprParser parser(&tokens);
    // tree::ParseTree* tree = parser.main();

    // auto s = tree->toStringTree(&parser);
    // auto input_stream = parser.getInputStream();
    // BOOST_CHECK(input_stream->size() == my_input.size());
}