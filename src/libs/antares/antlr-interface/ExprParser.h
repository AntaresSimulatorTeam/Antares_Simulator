
// Generated from Expr.g4 by ANTLR 4.13.1

#pragma once


#include "antlr4-runtime.h"




class  ExprParser : public antlr4::Parser {
public:
  enum {
    T__0 = 1, T__1 = 2, T__2 = 3, T__3 = 4, T__4 = 5, T__5 = 6, T__6 = 7, 
    T__7 = 8, T__8 = 9, NUMBER = 10, TIME = 11, IDENTIFIER = 12, COMPARISON = 13, 
    ADDSUB = 14, MULDIV = 15, LBRACKET = 16, RBRACKET = 17, WS = 18
  };

  enum {
    RuleFullexpr = 0, RuleShift = 1, RuleExpr = 2
  };

  explicit ExprParser(antlr4::TokenStream *input);

  ExprParser(antlr4::TokenStream *input, const antlr4::atn::ParserATNSimulatorOptions &options);

  ~ExprParser() override;

  std::string getGrammarFileName() const override;

  const antlr4::atn::ATN& getATN() const override;

  const std::vector<std::string>& getRuleNames() const override;

  const antlr4::dfa::Vocabulary& getVocabulary() const override;

  antlr4::atn::SerializedATNView getSerializedATN() const override;


  class FullexprContext;
  class ShiftContext;
  class ExprContext; 

  class  FullexprContext : public antlr4::ParserRuleContext {
  public:
    FullexprContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    ExprContext *expr();
    antlr4::tree::TerminalNode *EOF();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  FullexprContext* fullexpr();

  class  ShiftContext : public antlr4::ParserRuleContext {
  public:
    antlr4::Token *op = nullptr;
    ShiftContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *TIME();
    ExprContext *expr();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  ShiftContext* shift();

  class  ExprContext : public antlr4::ParserRuleContext {
  public:
    ExprContext(antlr4::ParserRuleContext *parent, size_t invokingState);
   
    ExprContext() = default;
    void copyFrom(ExprContext *context);
    using antlr4::ParserRuleContext::copyFrom;

    virtual size_t getRuleIndex() const override;

   
  };

  class  IdentifierContext : public ExprContext {
  public:
    IdentifierContext(ExprContext *ctx);

    antlr4::tree::TerminalNode *IDENTIFIER();

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
  };

  class  NegationContext : public ExprContext {
  public:
    NegationContext(ExprContext *ctx);

    ExprContext *expr();

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
  };

  class  ExpressionContext : public ExprContext {
  public:
    ExpressionContext(ExprContext *ctx);

    ExprContext *expr();

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
  };

  class  ComparisonContext : public ExprContext {
  public:
    ComparisonContext(ExprContext *ctx);

    std::vector<ExprContext *> expr();
    ExprContext* expr(size_t i);
    antlr4::tree::TerminalNode *COMPARISON();

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
  };

  class  AddsubContext : public ExprContext {
  public:
    AddsubContext(ExprContext *ctx);

    antlr4::Token *op = nullptr;
    std::vector<ExprContext *> expr();
    ExprContext* expr(size_t i);

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
  };

  class  PortFieldContext : public ExprContext {
  public:
    PortFieldContext(ExprContext *ctx);

    std::vector<antlr4::tree::TerminalNode *> IDENTIFIER();
    antlr4::tree::TerminalNode* IDENTIFIER(size_t i);

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
  };

  class  MuldivContext : public ExprContext {
  public:
    MuldivContext(ExprContext *ctx);

    antlr4::Token *op = nullptr;
    std::vector<ExprContext *> expr();
    ExprContext* expr(size_t i);

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
  };

  class  NumberContext : public ExprContext {
  public:
    NumberContext(ExprContext *ctx);

    antlr4::tree::TerminalNode *NUMBER();

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
  };

  class  TimeIndexContext : public ExprContext {
  public:
    TimeIndexContext(ExprContext *ctx);

    antlr4::tree::TerminalNode *IDENTIFIER();
    antlr4::tree::TerminalNode *LBRACKET();
    std::vector<ExprContext *> expr();
    ExprContext* expr(size_t i);
    antlr4::tree::TerminalNode *RBRACKET();

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
  };

  class  TimeShiftContext : public ExprContext {
  public:
    TimeShiftContext(ExprContext *ctx);

    antlr4::tree::TerminalNode *IDENTIFIER();
    antlr4::tree::TerminalNode *LBRACKET();
    std::vector<ShiftContext *> shift();
    ShiftContext* shift(size_t i);
    antlr4::tree::TerminalNode *RBRACKET();

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
  };

  class  FunctionContext : public ExprContext {
  public:
    FunctionContext(ExprContext *ctx);

    antlr4::tree::TerminalNode *IDENTIFIER();
    ExprContext *expr();

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
  };

  class  TimeShiftRangeContext : public ExprContext {
  public:
    TimeShiftRangeContext(ExprContext *ctx);

    ExprParser::ShiftContext *shift1 = nullptr;
    ExprParser::ShiftContext *shift2 = nullptr;
    antlr4::tree::TerminalNode *IDENTIFIER();
    antlr4::tree::TerminalNode *LBRACKET();
    antlr4::tree::TerminalNode *RBRACKET();
    std::vector<ShiftContext *> shift();
    ShiftContext* shift(size_t i);

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
  };

  class  TimeRangeContext : public ExprContext {
  public:
    TimeRangeContext(ExprContext *ctx);

    antlr4::tree::TerminalNode *IDENTIFIER();
    antlr4::tree::TerminalNode *LBRACKET();
    std::vector<ExprContext *> expr();
    ExprContext* expr(size_t i);
    antlr4::tree::TerminalNode *RBRACKET();

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
  };

  ExprContext* expr();
  ExprContext* expr(int precedence);

  bool sempred(antlr4::RuleContext *_localctx, size_t ruleIndex, size_t predicateIndex) override;

  bool exprSempred(ExprContext *_localctx, size_t predicateIndex);

  // By default the static state used to implement the parser is lazily initialized during the first
  // call to the constructor. You can call this function if you wish to initialize the static state
  // ahead of time.
  static void initialize();

private:
};

