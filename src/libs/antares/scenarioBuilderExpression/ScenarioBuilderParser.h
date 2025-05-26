
// Generated from ScenarioBuilder.g4 by ANTLR 4.13.2

#pragma once


#include "antlr4-runtime.h"




class  ScenarioBuilderParser : public antlr4::Parser {
public:
  enum {
    COMMA = 1, INT = 2, WS = 3, IDENTIFIER = 4, EQUALS = 5
  };

  enum {
    RuleRules = 0, RuleRule = 1, RuleGroup = 2, RuleYear = 3, RuleScenario = 4, 
    RuleHour = 5
  };

  explicit ScenarioBuilderParser(antlr4::TokenStream *input);

  ScenarioBuilderParser(antlr4::TokenStream *input, const antlr4::atn::ParserATNSimulatorOptions &options);

  ~ScenarioBuilderParser() override;

  std::string getGrammarFileName() const override;

  const antlr4::atn::ATN& getATN() const override;

  const std::vector<std::string>& getRuleNames() const override;

  const antlr4::dfa::Vocabulary& getVocabulary() const override;

  antlr4::atn::SerializedATNView getSerializedATN() const override;


  class RulesContext;
  class RuleContext;
  class GroupContext;
  class YearContext;
  class ScenarioContext;
  class HourContext; 

  class  RulesContext : public antlr4::ParserRuleContext {
  public:
    RulesContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *EOF();
    std::vector<RuleContext *> rule_();
    RuleContext* rule_(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  RulesContext* rules();

  class  RuleContext : public antlr4::ParserRuleContext {
  public:
    RuleContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    GroupContext *group();
    antlr4::tree::TerminalNode *COMMA();
    YearContext *year();
    antlr4::tree::TerminalNode *EQUALS();
    ScenarioContext *scenario();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  RuleContext* rule_();

  class  GroupContext : public antlr4::ParserRuleContext {
  public:
    GroupContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *IDENTIFIER();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  GroupContext* group();

  class  YearContext : public antlr4::ParserRuleContext {
  public:
    YearContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *INT();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  YearContext* year();

  class  ScenarioContext : public antlr4::ParserRuleContext {
  public:
    ScenarioContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *INT();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  ScenarioContext* scenario();

  class  HourContext : public antlr4::ParserRuleContext {
  public:
    HourContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *INT();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  HourContext* hour();


  // By default the static state used to implement the parser is lazily initialized during the first
  // call to the constructor. You can call this function if you wish to initialize the static state
  // ahead of time.
  static void initialize();

private:
};

