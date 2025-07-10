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

#include "include/antares/scenarioGroupParser/ScenarioGroupParser.h"

#include <ANTLRInputStream.h>
#include <fmt/format.h>

#include "antares/exception/RuntimeError.hpp"

#include "scenarioBuilderExpression/ScenarioBuilderBaseVisitor.h"
#include "scenarioBuilderExpression/ScenarioBuilderLexer.h"
#include "scenarioBuilderExpression/ScenarioBuilderParser.h"

namespace Antares
{
class ScenarioBuilderImplVisitor: public ScenarioBuilderBaseVisitor
{
public:
    std::any visitRules(ScenarioBuilderParser::RulesContext* ctx) override
    {
        return visitLine(ctx->line(0));
    }

    std::any visitLine(ScenarioBuilderParser::LineContext* ctx) override
    {
        auto group = std::any_cast<std::string>(visitGroup(ctx->group()));
        auto year = std::any_cast<int>(visitYear(ctx->year()));
        auto chronicle = std::any_cast<int>(visitChronicle(ctx->chronicle()));
        return ScenarioGroupParser::Line{.groupName = group, .year = year, .chronicle = chronicle};
    }

    std::any visitGroup(ScenarioBuilderParser::GroupContext* ctx) override
    {
        auto groupName = ctx->IDENTIFIER()->getText();
        return groupName;
    }

    std::any visitYear(ScenarioBuilderParser::YearContext* ctx) override
    {
        auto yearText = ctx->getText();
        int year = std::stoi(yearText);
        return year;
    }

    std::any visitChronicle(ScenarioBuilderParser::ChronicleContext* ctx) override
    {
        auto scenarioText = ctx->getText();
        int scenario = std::stoi(scenarioText);
        return scenario;
    }
};

ScenarioGroupParser::Line ScenarioGroupParser::parseLine(const std::string& line)
{
    antlr4::ANTLRInputStream input(line);
    ScenarioBuilderLexer lexer(&input);
    antlr4::CommonTokenStream tokens(&lexer);
    ScenarioBuilderParser parser(&tokens);
    try
    {
        auto* tree = parser->rules();
        ScenarioBuilderImplVisitor visitor;
        return std::any_cast<Line>(visitor.visit(tree));
    }
    catch (const std::exception& e)
    {
        throw Error::RuntimeError(
          fmt::format("Error parsing line \"{}\".\n\tDetails: {}", line, e.what()));
    }
}
} // namespace Antares
