
// Generated from ScenarioBuilder.g4 by ANTLR 4.13.2

#pragma once


#include "antlr4-runtime.h"
#include "ScenarioBuilderVisitor.h"


/**
 * This class provides an empty implementation of ScenarioBuilderVisitor, which can be
 * extended to create a visitor which only needs to handle a subset of the available methods.
 */
class  ScenarioBuilderBaseVisitor : public ScenarioBuilderVisitor {
public:

  virtual std::any visitRules(ScenarioBuilderParser::RulesContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitLine(ScenarioBuilderParser::LineContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitGroup(ScenarioBuilderParser::GroupContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitYear(ScenarioBuilderParser::YearContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitTimeSeriesNumber(ScenarioBuilderParser::TimeSeriesNumberContext *ctx) override {
    return visitChildren(ctx);
  }


};

