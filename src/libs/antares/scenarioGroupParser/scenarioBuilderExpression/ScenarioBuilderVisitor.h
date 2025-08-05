
// Generated from ScenarioBuilder.g4 by ANTLR 4.13.2

#pragma once


#include "antlr4-runtime.h"
#include "ScenarioBuilderParser.h"



/**
 * This class defines an abstract visitor for a parse tree
 * produced by ScenarioBuilderParser.
 */
class  ScenarioBuilderVisitor : public antlr4::tree::AbstractParseTreeVisitor {
public:

  /**
   * Visit parse trees produced by ScenarioBuilderParser.
   */
    virtual std::any visitRules(ScenarioBuilderParser::RulesContext *context) = 0;

    virtual std::any visitLine(ScenarioBuilderParser::LineContext *context) = 0;

    virtual std::any visitGroup(ScenarioBuilderParser::GroupContext *context) = 0;

    virtual std::any visitYear(ScenarioBuilderParser::YearContext *context) = 0;

    virtual std::any visitTimeSeriesNumber(ScenarioBuilderParser::TimeSeriesNumberContext *context) = 0;


};

