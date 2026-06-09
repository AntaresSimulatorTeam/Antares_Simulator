
// Generated from Expr.g4 by ANTLR 4.13.2

#pragma once


#include "antlr4-runtime.h"




class  ExprParser : public antlr4::Parser {
public:
  enum {
    T__0 = 1, T__1 = 2, T__2 = 3, T__3 = 4, T__4 = 5, T__5 = 6, T__6 = 7, 
    T__7 = 8, T__8 = 9, T__9 = 10, T__10 = 11, T__11 = 12, T__12 = 13, T__13 = 14, 
    NUMBER = 15, TIME = 16, IDENTIFIER = 17, COMPARISON = 18, WS = 19
  };

  enum {
    RulePortFieldExpr = 0, RuleFullexpr = 1, RuleSum_bound = 2, RuleExpr = 3, 
    RuleArgList = 4, RuleAtom = 5, RuleShift = 6, RuleShift_expr = 7, RuleRight_expr = 8
  };

  explicit ExprParser(antlr4::TokenStream *input);

  ExprParser(antlr4::TokenStream *input, const antlr4::atn::ParserATNSimulatorOptions &options);

  ~ExprParser() override;

  std::string getGrammarFileName() const override;

  const antlr4::atn::ATN& getATN() const override;

  const std::vector<std::string>& getRuleNames() const override;

  const antlr4::dfa::Vocabulary& getVocabulary() const override;

  antlr4::atn::SerializedATNView getSerializedATN() const override;


  class PortFieldExprContext;
  class FullexprContext;
  class Sum_boundContext;
  class ExprContext;
  class ArgListContext;
  class AtomContext;
  class ShiftContext;
  class Shift_exprContext;
  class Right_exprContext; 

  class  PortFieldExprContext : public antlr4::ParserRuleContext {
  public:
    PortFieldExprContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<antlr4::tree::TerminalNode *> IDENTIFIER();
    antlr4::tree::TerminalNode* IDENTIFIER(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  PortFieldExprContext* portFieldExpr();

  class  FullexprContext : public antlr4::ParserRuleContext {
  public:
    FullexprContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    ExprContext *expr();
    antlr4::tree::TerminalNode *EOF();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  FullexprContext* fullexpr();

  class  Sum_boundContext : public antlr4::ParserRuleContext {
  public:
    Sum_boundContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    ExprContext *expr();
    ShiftContext *shift();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Sum_boundContext* sum_bound();

  class  ExprContext : public antlr4::ParserRuleContext {
  public:
    ExprContext(antlr4::ParserRuleContext *parent, size_t invokingState);
   
    ExprContext() = default;
    void copyFrom(ExprContext *context);
    using antlr4::ParserRuleContext::copyFrom;

    virtual size_t getRuleIndex() const override;

   
  };

  class  PortFieldSumContext : public ExprContext {
  public:
    PortFieldSumContext(ExprContext *ctx);

    PortFieldExprContext *portFieldExpr();

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
  };

  class  NegationContext : public ExprContext {
  public:
    NegationContext(ExprContext *ctx);

    ExprContext *expr();

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
  };

  class  UnsignedAtomContext : public ExprContext {
  public:
    UnsignedAtomContext(ExprContext *ctx);

    AtomContext *atom();

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

  class  AllTimeSumContext : public ExprContext {
  public:
    AllTimeSumContext(ExprContext *ctx);

    ExprContext *expr();

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
  };

  class  TimeIndexExprContext : public ExprContext {
  public:
    TimeIndexExprContext(ExprContext *ctx);

    std::vector<ExprContext *> expr();
    ExprContext* expr(size_t i);

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

  class  TimeShiftExprContext : public ExprContext {
  public:
    TimeShiftExprContext(ExprContext *ctx);

    ExprContext *expr();
    ShiftContext *shift();

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
  };

  class  PortFieldContext : public ExprContext {
  public:
    PortFieldContext(ExprContext *ctx);

    PortFieldExprContext *portFieldExpr();

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

  class  TimeSumContext : public ExprContext {
  public:
    TimeSumContext(ExprContext *ctx);

    ExprParser::Sum_boundContext *from = nullptr;
    ExprParser::Sum_boundContext *to = nullptr;
    ExprContext *expr();
    std::vector<Sum_boundContext *> sum_bound();
    Sum_boundContext* sum_bound(size_t i);

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
  };

  class  TimeIndexContext : public ExprContext {
  public:
    TimeIndexContext(ExprContext *ctx);

    antlr4::tree::TerminalNode *IDENTIFIER();
    ExprContext *expr();

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
  };

  class  TimeShiftContext : public ExprContext {
  public:
    TimeShiftContext(ExprContext *ctx);

    antlr4::tree::TerminalNode *IDENTIFIER();
    ShiftContext *shift();

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
  };

  class  FunctionContext : public ExprContext {
  public:
    FunctionContext(ExprContext *ctx);

    antlr4::tree::TerminalNode *IDENTIFIER();
    ArgListContext *argList();

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
  };

  class  PowerContext : public ExprContext {
  public:
    PowerContext(ExprContext *ctx);

    std::vector<ExprContext *> expr();
    ExprContext* expr(size_t i);

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
  };

  ExprContext* expr();
  ExprContext* expr(int precedence);
  class  ArgListContext : public antlr4::ParserRuleContext {
  public:
    ArgListContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<ExprContext *> expr();
    ExprContext* expr(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  ArgListContext* argList();

  class  AtomContext : public antlr4::ParserRuleContext {
  public:
    AtomContext(antlr4::ParserRuleContext *parent, size_t invokingState);
   
    AtomContext() = default;
    void copyFrom(AtomContext *context);
    using antlr4::ParserRuleContext::copyFrom;

    virtual size_t getRuleIndex() const override;

   
  };

  class  NumberContext : public AtomContext {
  public:
    NumberContext(AtomContext *ctx);

    antlr4::tree::TerminalNode *NUMBER();

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
  };

  class  IdentifierContext : public AtomContext {
  public:
    IdentifierContext(AtomContext *ctx);

    antlr4::tree::TerminalNode *IDENTIFIER();

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
  };

  AtomContext* atom();

  class  ShiftContext : public antlr4::ParserRuleContext {
  public:
    ShiftContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *TIME();
    Shift_exprContext *shift_expr();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  ShiftContext* shift();

  class  Shift_exprContext : public antlr4::ParserRuleContext {
  public:
    Shift_exprContext(antlr4::ParserRuleContext *parent, size_t invokingState);
   
    Shift_exprContext() = default;
    void copyFrom(Shift_exprContext *context);
    using antlr4::ParserRuleContext::copyFrom;

    virtual size_t getRuleIndex() const override;

   
  };

  class  SignedAtomContext : public Shift_exprContext {
  public:
    SignedAtomContext(Shift_exprContext *ctx);

    antlr4::Token *op = nullptr;
    AtomContext *atom();

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
  };

  class  SignedExpressionContext : public Shift_exprContext {
  public:
    SignedExpressionContext(Shift_exprContext *ctx);

    antlr4::Token *op = nullptr;
    ExprContext *expr();

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
  };

  class  ShiftMuldivContext : public Shift_exprContext {
  public:
    ShiftMuldivContext(Shift_exprContext *ctx);

    antlr4::Token *op = nullptr;
    Shift_exprContext *shift_expr();
    Right_exprContext *right_expr();

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
  };

  class  ShiftAddsubContext : public Shift_exprContext {
  public:
    ShiftAddsubContext(Shift_exprContext *ctx);

    antlr4::Token *op = nullptr;
    Shift_exprContext *shift_expr();
    Right_exprContext *right_expr();

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
  };

  class  ShiftPowerContext : public Shift_exprContext {
  public:
    ShiftPowerContext(Shift_exprContext *ctx);

    Shift_exprContext *shift_expr();
    Right_exprContext *right_expr();

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
  };

  Shift_exprContext* shift_expr();
  Shift_exprContext* shift_expr(int precedence);
  class  Right_exprContext : public antlr4::ParserRuleContext {
  public:
    Right_exprContext(antlr4::ParserRuleContext *parent, size_t invokingState);
   
    Right_exprContext() = default;
    void copyFrom(Right_exprContext *context);
    using antlr4::ParserRuleContext::copyFrom;

    virtual size_t getRuleIndex() const override;

   
  };

  class  RightExpressionContext : public Right_exprContext {
  public:
    RightExpressionContext(Right_exprContext *ctx);

    ExprContext *expr();

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
  };

  class  RightMuldivContext : public Right_exprContext {
  public:
    RightMuldivContext(Right_exprContext *ctx);

    antlr4::Token *op = nullptr;
    std::vector<Right_exprContext *> right_expr();
    Right_exprContext* right_expr(size_t i);

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
  };

  class  RightAtomContext : public Right_exprContext {
  public:
    RightAtomContext(Right_exprContext *ctx);

    AtomContext *atom();

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
  };

  class  RightPowerContext : public Right_exprContext {
  public:
    RightPowerContext(Right_exprContext *ctx);

    std::vector<Right_exprContext *> right_expr();
    Right_exprContext* right_expr(size_t i);

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
  };

  Right_exprContext* right_expr();
  Right_exprContext* right_expr(int precedence);

  bool sempred(antlr4::RuleContext *_localctx, size_t ruleIndex, size_t predicateIndex) override;

  bool exprSempred(ExprContext *_localctx, size_t predicateIndex);
  bool shift_exprSempred(Shift_exprContext *_localctx, size_t predicateIndex);
  bool right_exprSempred(Right_exprContext *_localctx, size_t predicateIndex);

  // By default the static state used to implement the parser is lazily initialized during the first
  // call to the constructor. You can call this function if you wish to initialize the static state
  // ahead of time.
  static void initialize();

private:
};

