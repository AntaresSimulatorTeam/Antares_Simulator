// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

// this is needed to avoid macro redefinition conflicts on Windows
#ifdef _WIN32
#pragma push_macro("ERROR")
#undef ERROR
#endif
#include <HoursFieldBaseVisitor.h>
//
#include <BaseErrorListener.h>

#include "HoursFieldLexer.h"
//
#include "antares/study/parts/short-term-storage/makeGroupsOfHoursFromString.h"
#ifdef _WIN32
#pragma pop_macro("ERROR")
#endif
namespace Antares::Data::ShortTermStorage
{

class HoursCollectorVisitor: public HoursFieldBaseVisitor
{
public:
    std::any visitHoursField(HoursFieldParser::HoursFieldContext* ctx) override
    {
        std::vector<std::set<int>> result;
        for (auto groupCtx: ctx->group())
        {
            result.push_back(std::any_cast<std::set<int>>(visit(groupCtx)));
        }
        return result;
    }

    std::any visitGroup(HoursFieldParser::GroupContext* ctx) override
    {
        std::set<int> hours;
        for (auto hourCtx: ctx->hour())
        {
            hours.insert(std::stoi(hourCtx->getText()));
        }
        return hours;
    }
};

class CustomErrorListener final: public antlr4::BaseErrorListener
{
public:
    void syntaxError([[maybe_unused]] antlr4::Recognizer* recognizer,
                     antlr4::Token* offendingSymbol,
                     size_t line,
                     size_t charPositionInLine,
                     const std::string& msg,
                     [[maybe_unused]] std::exception_ptr e) override
    {
        std::ostringstream os;
        os << "Syntax error at line " << line << ":" << charPositionInLine << " - " << msg
           << std::endl;
        if (offendingSymbol)
        {
            os << "Offending symbol: " << offendingSymbol->getText() << std::endl;
        }
        throw ShortTermStorageAdditionalConstraintsError(os.str());
    }
};

class GroupsHours final
{
public:
    explicit GroupsHours(const std::string& hoursField):
        hoursField_(hoursField),
        stream_(hoursField_),
        lexer_(&stream_),
        tokens_(&lexer_),
        parser_(&tokens_)
    {
        lexer_.removeErrorListeners();
        lexer_.addErrorListener(&customErrorListener_);
        parser_.removeErrorListeners();
        parser_.addErrorListener(&customErrorListener_);
    }

    auto* hoursField()
    {
        return parser_.hoursField();
    }

private:
    std::string hoursField_;
    CustomErrorListener customErrorListener_;
    antlr4::ANTLRInputStream stream_;
    HoursFieldLexer lexer_;
    antlr4::CommonTokenStream tokens_;
    HoursFieldParser parser_;
};

std::vector<std::set<int>> makeGroupsOfHours(const std::string& hoursField)
{
    GroupsHours groupsHours(hoursField);
    auto* tree = groupsHours.hoursField();
    try
    {
        HoursCollectorVisitor visitor;
        return std::any_cast<std::vector<std::set<int>>>(visitor.visit(tree));
    }
    catch (std::exception& e)
    {
        std::ostringstream os;
        os << "Exception thrown while parsing '" << hoursField << "' : " << e.what() << std::endl;
        throw ShortTermStorageAdditionalConstraintsError(os.str());
    }
}
} // namespace Antares::Data::ShortTermStorage
