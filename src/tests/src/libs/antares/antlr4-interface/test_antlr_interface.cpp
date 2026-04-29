// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#define BOOST_TEST_MODULE antlr_interface tests
#include <iostream>

#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>

#include "ExprLexer.h"
#include "ExprParser.h"
#include "antlr4-runtime.h"

using namespace antlr4;

BOOST_AUTO_TEST_CASE(test_antlr_interface)
{
    const std::string my_input = "y = b + a*x";
    ANTLRInputStream input(my_input);
    ExprLexer lexer(&input);
    CommonTokenStream tokens(&lexer);
    tokens.fill();
    BOOST_CHECK(tokens.getTokens().size() == 8);
}
