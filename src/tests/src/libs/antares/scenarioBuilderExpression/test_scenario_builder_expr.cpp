// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#define BOOST_TEST_MODULE ScenarioBuilderExpression
#define WIN32_LEAN_AND_MEAN
#include <ANTLRInputStream.h>
#include <CommonTokenStream.h>
#include <ScenarioBuilderLexer.h>
#include <ScenarioBuilderParser.h>

#include <boost/test/unit_test.hpp>

#include "antares/scenarioGroupParser/ScenarioGroupParser.h"

#include "ScenarioBuilderBaseVisitor.h"

using namespace std::string_literals;
using namespace Antares;

class CustomErrorListener: public antlr4::BaseErrorListener
{
    void syntaxError([[maybe_unused]] antlr4::Recognizer* recognizer,
                     [[maybe_unused]] antlr4::Token* offendingSymbol,
                     [[maybe_unused]] size_t line,
                     [[maybe_unused]] size_t charPositionInLine,
                     [[maybe_unused]] const std::string& msg,
                     [[maybe_unused]] std::exception_ptr e) override
    {
        throw std::runtime_error("plop");
    }
};

struct ScenarioBuilderFixture
{
    ScenarioGroupParser group_parser;
    CustomErrorListener customErrorListener;
};

BOOST_FIXTURE_TEST_CASE(dont_accept_strings_for_year, ScenarioBuilderFixture)
{
    BOOST_REQUIRE_THROW(group_parser.parseLine("groupe,year=0"s, &customErrorListener),
                        std::runtime_error); // Expecting a runtime error due to mismatched input
}

BOOST_FIXTURE_TEST_CASE(dont_accept_strings_for_scenario, ScenarioBuilderFixture)
{
    BOOST_REQUIRE_THROW(group_parser.parseLine("groupe,0=scenario"s, &customErrorListener),
                        std::runtime_error); // Expecting a runtime error due to mismatched input
}

BOOST_FIXTURE_TEST_CASE(group_name_parsed_properly, ScenarioBuilderFixture)
{
    const auto line = group_parser.parseLine("groupe,3=56"s);
    BOOST_CHECK_EQUAL(line.groupName, "groupe");
}

BOOST_FIXTURE_TEST_CASE(year_parsed_properly, ScenarioBuilderFixture)
{
    const auto line = group_parser.parseLine("groupe,67=1"s);
    BOOST_CHECK_EQUAL(line.year, 67);
}

BOOST_FIXTURE_TEST_CASE(year_scenario_properly, ScenarioBuilderFixture)
{
    const auto line = group_parser.parseLine("groupe,67=1"s);
    BOOST_CHECK_EQUAL(line.timeSeriesNumber, 1);
}

BOOST_FIXTURE_TEST_CASE(parse_all_properly, ScenarioBuilderFixture)
{
    const auto [groupName, year, timeSeriesNumber] = group_parser.parseLine(
      "  groupe_name   ,42 =      9999     "s);
    BOOST_CHECK_EQUAL(groupName, "groupe_name");
    BOOST_CHECK_EQUAL(year, 42);
    BOOST_CHECK_EQUAL(timeSeriesNumber, 9999);
}
