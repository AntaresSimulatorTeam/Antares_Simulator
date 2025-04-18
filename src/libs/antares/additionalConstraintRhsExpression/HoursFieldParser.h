
// Generated from HoursField.g4 by ANTLR 4.13.2

#pragma once


#include "antlr4-runtime.h"




class  HoursFieldParser : public antlr4::Parser {
public:
  enum {
    LBRACK = 1, RBRACK = 2, COMMA = 3, INT = 4, WS = 5
  };

  enum {
    RuleHoursField = 0, RuleGroup = 1, RuleHour = 2
  };

  explicit HoursFieldParser(antlr4::TokenStream *input);

  HoursFieldParser(antlr4::TokenStream *input, const antlr4::atn::ParserATNSimulatorOptions &options);

  ~HoursFieldParser() override;

  std::string getGrammarFileName() const override;

  const antlr4::atn::ATN& getATN() const override;

  const std::vector<std::string>& getRuleNames() const override;

  const antlr4::dfa::Vocabulary& getVocabulary() const override;

  antlr4::atn::SerializedATNView getSerializedATN() const override;


  class HoursFieldContext;
  class GroupContext;
  class HourContext; 

  class  HoursFieldContext : public antlr4::ParserRuleContext {
  public:
    HoursFieldContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<GroupContext *> group();
    GroupContext* group(size_t i);
    antlr4::tree::TerminalNode *EOF();
    std::vector<antlr4::tree::TerminalNode *> COMMA();
    antlr4::tree::TerminalNode* COMMA(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  HoursFieldContext* hoursField();

  class  GroupContext : public antlr4::ParserRuleContext {
  public:
    GroupContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *LBRACK();
    std::vector<HourContext *> hour();
    HourContext* hour(size_t i);
    antlr4::tree::TerminalNode *RBRACK();
    std::vector<antlr4::tree::TerminalNode *> WS();
    antlr4::tree::TerminalNode* WS(size_t i);
    std::vector<antlr4::tree::TerminalNode *> COMMA();
    antlr4::tree::TerminalNode* COMMA(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  GroupContext* group();

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

