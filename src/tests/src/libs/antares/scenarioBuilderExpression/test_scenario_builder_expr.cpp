// Copyright 2007-2025, RTE (https://www.rte-france.com)
// See AUTHORS.txt
// SPDX-License-Identifier: MPL-2.0
// This file is part of Antares-Simulator,
// Adequacy and Performance assessment for interconnected energy networks.
//
// Antares_Simulator is free software: you can redistribute it and/or modify
// it under the terms of the Mozilla Public Licence 2.0 as published by
// the Mozilla Foundation, either version 2 of the License, or
// (at your option) any later version.
//
// Antares_Simulator is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// Mozilla Public Licence 2.0 for more details.
//
// You should have received a copy of the Mozilla Public Licence 2.0
// along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.

#define BOOST_TEST_MODULE ScenarioBuilderExpression
#define WIN32_LEAN_AND_MEAN
#include <ANTLRInputStream.h>
#include <CommonTokenStream.h>
#include <boost/test/unit_test.hpp>
#include <ScenarioBuilderLexer.h>
#include <ScenarioBuilderParser.h>

#include "ScenarioBuilderBaseVisitor.h"
#include "ScenarioGroupParser.h"

using namespace std::string_literals;
using namespace Antares;

class CustomErrorListener : public antlr4::BaseErrorListener {
    void syntaxError(antlr4::Recognizer *recognizer, antlr4::Token *offendingSymbol, size_t line,
            size_t charPositionInLine, const std::string &msg, std::exception_ptr e) override {
        throw std::runtime_error("plop");
    };
};

struct ScenarioBuilderFixture {
    ScenarioGroupParser group_parser;
    CustomErrorListener customErrorListener;

    std::unique_ptr<antlr4::ANTLRInputStream> input;
    std::unique_ptr<ScenarioBuilderLexer> lexer;
    std::unique_ptr<antlr4::CommonTokenStream> tokens;
    std::unique_ptr<ScenarioBuilderParser> parser;

    void prepare(const std::string& line, bool withErrorListener = false) {
        input = std::make_unique<antlr4::ANTLRInputStream>(line);
        lexer = std::make_unique<ScenarioBuilderLexer>(input.get());
        tokens = std::make_unique<antlr4::CommonTokenStream>(lexer.get());
        parser = std::make_unique<ScenarioBuilderParser>(tokens.get());
        if (withErrorListener) {
            lexer->removeErrorListeners();
            parser->removeErrorListeners();
            lexer->addErrorListener(&customErrorListener);
            parser->addErrorListener(&customErrorListener);
        }
    }
};

BOOST_FIXTURE_TEST_CASE(simple_line_parsed_properly, ScenarioBuilderFixture) {
    prepare("groupe,0=0"s);
    auto tree = parser->rules();
    BOOST_REQUIRE(tree != nullptr);
}

BOOST_FIXTURE_TEST_CASE(whitespaces_parsed_properly, ScenarioBuilderFixture) {
    prepare("  groupe   ,0 =      0     "s);
    auto tree = parser->rules();
    BOOST_REQUIRE(tree != nullptr);
}

BOOST_FIXTURE_TEST_CASE(dont_accept_strings_for_year, ScenarioBuilderFixture) {
    prepare("groupe,year=0"s, true);
    BOOST_REQUIRE_THROW(
        parser->rules(), std::runtime_error
    ); // Expecting a runtime error due to mismatched input
}

BOOST_FIXTURE_TEST_CASE(dont_accept_strings_for_scenario, ScenarioBuilderFixture) {
    prepare("groupe,0=scenario"s, true);
    BOOST_REQUIRE_THROW(
        parser->rules(), std::runtime_error
    ); // Expecting a runtime error due to mismatched input
}

BOOST_FIXTURE_TEST_CASE(group_name_parsed_properly, ScenarioBuilderFixture) {
    const auto line = group_parser.parseLine("groupe,3=56"s);
    BOOST_CHECK_EQUAL(line.groupName, "groupe");
}

BOOST_FIXTURE_TEST_CASE(year_parsed_properly, ScenarioBuilderFixture) {
    const auto line = group_parser.parseLine("groupe,67=1"s);
    BOOST_CHECK_EQUAL(line.year, 67);
}

BOOST_FIXTURE_TEST_CASE(year_scenario_properly, ScenarioBuilderFixture) {
    const auto line = group_parser.parseLine("groupe,67=1"s);
    BOOST_CHECK_EQUAL(line.chronicle, 1);
}

BOOST_FIXTURE_TEST_CASE(parse_all_properly, ScenarioBuilderFixture) {
    const auto [groupName, year, chronicle] = group_parser.parseLine("  groupe_name   ,42 =      9999     "s);
    BOOST_REQUIRE_EQUAL(groupName, "groupe_name");
    BOOST_REQUIRE_EQUAL(year, 42);
    BOOST_REQUIRE_EQUAL(chronicle, 9999);
}