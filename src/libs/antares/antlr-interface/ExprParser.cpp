
// Generated from Expr.g4 by ANTLR 4.7.2


#include "ExprVisitor.h"

#include "ExprParser.h"


using namespace antlrcpp;
using namespace antlr4;

ExprParser::ExprParser(TokenStream *input) : Parser(input) {
  _interpreter = new atn::ParserATNSimulator(this, _atn, _decisionToDFA, _sharedContextCache);
}

ExprParser::~ExprParser() {
  delete _interpreter;
}

std::string ExprParser::getGrammarFileName() const {
  return "Expr.g4";
}

const std::vector<std::string>& ExprParser::getRuleNames() const {
  return _ruleNames;
}

dfa::Vocabulary& ExprParser::getVocabulary() const {
  return _vocabulary;
}


//----------------- FullexprContext ------------------------------------------------------------------

ExprParser::FullexprContext::FullexprContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

ExprParser::ExprContext* ExprParser::FullexprContext::expr() {
  return getRuleContext<ExprParser::ExprContext>(0);
}

tree::TerminalNode* ExprParser::FullexprContext::EOF() {
  return getToken(ExprParser::EOF, 0);
}


size_t ExprParser::FullexprContext::getRuleIndex() const {
  return ExprParser::RuleFullexpr;
}

antlrcpp::Any ExprParser::FullexprContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<ExprVisitor*>(visitor))
    return parserVisitor->visitFullexpr(this);
  else
    return visitor->visitChildren(this);
}

ExprParser::FullexprContext* ExprParser::fullexpr() {
  FullexprContext *_localctx = _tracker.createInstance<FullexprContext>(_ctx, getState());
  enterRule(_localctx, 0, ExprParser::RuleFullexpr);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(12);
    expr(0);
    setState(13);
    match(ExprParser::EOF);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ExprContext ------------------------------------------------------------------

ExprParser::ExprContext::ExprContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}


size_t ExprParser::ExprContext::getRuleIndex() const {
  return ExprParser::RuleExpr;
}

void ExprParser::ExprContext::copyFrom(ExprContext *ctx) {
  ParserRuleContext::copyFrom(ctx);
}

//----------------- TimeSumContext ------------------------------------------------------------------

ExprParser::ExprContext* ExprParser::TimeSumContext::expr() {
  return getRuleContext<ExprParser::ExprContext>(0);
}

std::vector<ExprParser::ShiftContext *> ExprParser::TimeSumContext::shift() {
  return getRuleContexts<ExprParser::ShiftContext>();
}

ExprParser::ShiftContext* ExprParser::TimeSumContext::shift(size_t i) {
  return getRuleContext<ExprParser::ShiftContext>(i);
}

ExprParser::TimeSumContext::TimeSumContext(ExprContext *ctx) { copyFrom(ctx); }

antlrcpp::Any ExprParser::TimeSumContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<ExprVisitor*>(visitor))
    return parserVisitor->visitTimeSum(this);
  else
    return visitor->visitChildren(this);
}
//----------------- NegationContext ------------------------------------------------------------------

ExprParser::ExprContext* ExprParser::NegationContext::expr() {
  return getRuleContext<ExprParser::ExprContext>(0);
}

ExprParser::NegationContext::NegationContext(ExprContext *ctx) { copyFrom(ctx); }

antlrcpp::Any ExprParser::NegationContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<ExprVisitor*>(visitor))
    return parserVisitor->visitNegation(this);
  else
    return visitor->visitChildren(this);
}
//----------------- UnsignedAtomContext ------------------------------------------------------------------

ExprParser::AtomContext* ExprParser::UnsignedAtomContext::atom() {
  return getRuleContext<ExprParser::AtomContext>(0);
}

ExprParser::UnsignedAtomContext::UnsignedAtomContext(ExprContext *ctx) { copyFrom(ctx); }

antlrcpp::Any ExprParser::UnsignedAtomContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<ExprVisitor*>(visitor))
    return parserVisitor->visitUnsignedAtom(this);
  else
    return visitor->visitChildren(this);
}
//----------------- ExpressionContext ------------------------------------------------------------------

ExprParser::ExprContext* ExprParser::ExpressionContext::expr() {
  return getRuleContext<ExprParser::ExprContext>(0);
}

ExprParser::ExpressionContext::ExpressionContext(ExprContext *ctx) { copyFrom(ctx); }

antlrcpp::Any ExprParser::ExpressionContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<ExprVisitor*>(visitor))
    return parserVisitor->visitExpression(this);
  else
    return visitor->visitChildren(this);
}
//----------------- TimeIndexContext ------------------------------------------------------------------

tree::TerminalNode* ExprParser::TimeIndexContext::IDENTIFIER() {
  return getToken(ExprParser::IDENTIFIER, 0);
}

ExprParser::ExprContext* ExprParser::TimeIndexContext::expr() {
  return getRuleContext<ExprParser::ExprContext>(0);
}

ExprParser::TimeIndexContext::TimeIndexContext(ExprContext *ctx) { copyFrom(ctx); }

antlrcpp::Any ExprParser::TimeIndexContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<ExprVisitor*>(visitor))
    return parserVisitor->visitTimeIndex(this);
  else
    return visitor->visitChildren(this);
}
//----------------- ComparisonContext ------------------------------------------------------------------

std::vector<ExprParser::ExprContext *> ExprParser::ComparisonContext::expr() {
  return getRuleContexts<ExprParser::ExprContext>();
}

ExprParser::ExprContext* ExprParser::ComparisonContext::expr(size_t i) {
  return getRuleContext<ExprParser::ExprContext>(i);
}

tree::TerminalNode* ExprParser::ComparisonContext::COMPARISON() {
  return getToken(ExprParser::COMPARISON, 0);
}

ExprParser::ComparisonContext::ComparisonContext(ExprContext *ctx) { copyFrom(ctx); }

antlrcpp::Any ExprParser::ComparisonContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<ExprVisitor*>(visitor))
    return parserVisitor->visitComparison(this);
  else
    return visitor->visitChildren(this);
}
//----------------- AllTimeSumContext ------------------------------------------------------------------

ExprParser::ExprContext* ExprParser::AllTimeSumContext::expr() {
  return getRuleContext<ExprParser::ExprContext>(0);
}

ExprParser::AllTimeSumContext::AllTimeSumContext(ExprContext *ctx) { copyFrom(ctx); }

antlrcpp::Any ExprParser::AllTimeSumContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<ExprVisitor*>(visitor))
    return parserVisitor->visitAllTimeSum(this);
  else
    return visitor->visitChildren(this);
}
//----------------- TimeShiftContext ------------------------------------------------------------------

tree::TerminalNode* ExprParser::TimeShiftContext::IDENTIFIER() {
  return getToken(ExprParser::IDENTIFIER, 0);
}

ExprParser::ShiftContext* ExprParser::TimeShiftContext::shift() {
  return getRuleContext<ExprParser::ShiftContext>(0);
}

ExprParser::TimeShiftContext::TimeShiftContext(ExprContext *ctx) { copyFrom(ctx); }

antlrcpp::Any ExprParser::TimeShiftContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<ExprVisitor*>(visitor))
    return parserVisitor->visitTimeShift(this);
  else
    return visitor->visitChildren(this);
}
//----------------- FunctionContext ------------------------------------------------------------------

tree::TerminalNode* ExprParser::FunctionContext::IDENTIFIER() {
  return getToken(ExprParser::IDENTIFIER, 0);
}

ExprParser::ExprContext* ExprParser::FunctionContext::expr() {
  return getRuleContext<ExprParser::ExprContext>(0);
}

ExprParser::FunctionContext::FunctionContext(ExprContext *ctx) { copyFrom(ctx); }

antlrcpp::Any ExprParser::FunctionContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<ExprVisitor*>(visitor))
    return parserVisitor->visitFunction(this);
  else
    return visitor->visitChildren(this);
}
//----------------- AddsubContext ------------------------------------------------------------------

std::vector<ExprParser::ExprContext *> ExprParser::AddsubContext::expr() {
  return getRuleContexts<ExprParser::ExprContext>();
}

ExprParser::ExprContext* ExprParser::AddsubContext::expr(size_t i) {
  return getRuleContext<ExprParser::ExprContext>(i);
}

ExprParser::AddsubContext::AddsubContext(ExprContext *ctx) { copyFrom(ctx); }

antlrcpp::Any ExprParser::AddsubContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<ExprVisitor*>(visitor))
    return parserVisitor->visitAddsub(this);
  else
    return visitor->visitChildren(this);
}
//----------------- PortFieldContext ------------------------------------------------------------------

std::vector<tree::TerminalNode *> ExprParser::PortFieldContext::IDENTIFIER() {
  return getTokens(ExprParser::IDENTIFIER);
}

tree::TerminalNode* ExprParser::PortFieldContext::IDENTIFIER(size_t i) {
  return getToken(ExprParser::IDENTIFIER, i);
}

ExprParser::PortFieldContext::PortFieldContext(ExprContext *ctx) { copyFrom(ctx); }

antlrcpp::Any ExprParser::PortFieldContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<ExprVisitor*>(visitor))
    return parserVisitor->visitPortField(this);
  else
    return visitor->visitChildren(this);
}
//----------------- MuldivContext ------------------------------------------------------------------

std::vector<ExprParser::ExprContext *> ExprParser::MuldivContext::expr() {
  return getRuleContexts<ExprParser::ExprContext>();
}

ExprParser::ExprContext* ExprParser::MuldivContext::expr(size_t i) {
  return getRuleContext<ExprParser::ExprContext>(i);
}

ExprParser::MuldivContext::MuldivContext(ExprContext *ctx) { copyFrom(ctx); }

antlrcpp::Any ExprParser::MuldivContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<ExprVisitor*>(visitor))
    return parserVisitor->visitMuldiv(this);
  else
    return visitor->visitChildren(this);
}

ExprParser::ExprContext* ExprParser::expr() {
   return expr(0);
}

ExprParser::ExprContext* ExprParser::expr(int precedence) {
  ParserRuleContext *parentContext = _ctx;
  size_t parentState = getState();
  ExprParser::ExprContext *_localctx = _tracker.createInstance<ExprContext>(_ctx, parentState);
  ExprParser::ExprContext *previousContext = _localctx;
  (void)previousContext; // Silence compiler, in case the context is not used by generated code.
  size_t startState = 2;
  enterRecursionRule(_localctx, 2, ExprParser::RuleExpr, precedence);

    size_t _la = 0;

  auto onExit = finally([=] {
    unrollRecursionContexts(parentContext);
  });
  try {
    size_t alt;
    enterOuterAlt(_localctx, 1);
    setState(55);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 0, _ctx)) {
    case 1: {
      _localctx = _tracker.createInstance<UnsignedAtomContext>(_localctx);
      _ctx = _localctx;
      previousContext = _localctx;

      setState(16);
      atom();
      break;
    }

    case 2: {
      _localctx = _tracker.createInstance<PortFieldContext>(_localctx);
      _ctx = _localctx;
      previousContext = _localctx;
      setState(17);
      match(ExprParser::IDENTIFIER);
      setState(18);
      match(ExprParser::T__0);
      setState(19);
      match(ExprParser::IDENTIFIER);
      break;
    }

    case 3: {
      _localctx = _tracker.createInstance<NegationContext>(_localctx);
      _ctx = _localctx;
      previousContext = _localctx;
      setState(20);
      match(ExprParser::T__1);
      setState(21);
      expr(10);
      break;
    }

    case 4: {
      _localctx = _tracker.createInstance<ExpressionContext>(_localctx);
      _ctx = _localctx;
      previousContext = _localctx;
      setState(22);
      match(ExprParser::T__2);
      setState(23);
      expr(0);
      setState(24);
      match(ExprParser::T__3);
      break;
    }

    case 5: {
      _localctx = _tracker.createInstance<AllTimeSumContext>(_localctx);
      _ctx = _localctx;
      previousContext = _localctx;
      setState(26);
      match(ExprParser::T__7);
      setState(27);
      match(ExprParser::T__2);
      setState(28);
      expr(0);
      setState(29);
      match(ExprParser::T__3);
      break;
    }

    case 6: {
      _localctx = _tracker.createInstance<TimeSumContext>(_localctx);
      _ctx = _localctx;
      previousContext = _localctx;
      setState(31);
      match(ExprParser::T__7);
      setState(32);
      match(ExprParser::T__2);
      setState(33);
      dynamic_cast<TimeSumContext *>(_localctx)->from = shift();
      setState(34);
      match(ExprParser::T__8);
      setState(35);
      dynamic_cast<TimeSumContext *>(_localctx)->to = shift();
      setState(36);
      match(ExprParser::T__9);
      setState(37);
      expr(0);
      setState(38);
      match(ExprParser::T__3);
      break;
    }

    case 7: {
      _localctx = _tracker.createInstance<FunctionContext>(_localctx);
      _ctx = _localctx;
      previousContext = _localctx;
      setState(40);
      match(ExprParser::IDENTIFIER);
      setState(41);
      match(ExprParser::T__2);
      setState(42);
      expr(0);
      setState(43);
      match(ExprParser::T__3);
      break;
    }

    case 8: {
      _localctx = _tracker.createInstance<TimeShiftContext>(_localctx);
      _ctx = _localctx;
      previousContext = _localctx;
      setState(45);
      match(ExprParser::IDENTIFIER);
      setState(46);
      match(ExprParser::T__10);
      setState(47);
      shift();
      setState(48);
      match(ExprParser::T__11);
      break;
    }

    case 9: {
      _localctx = _tracker.createInstance<TimeIndexContext>(_localctx);
      _ctx = _localctx;
      previousContext = _localctx;
      setState(50);
      match(ExprParser::IDENTIFIER);
      setState(51);
      match(ExprParser::T__10);
      setState(52);
      expr(0);
      setState(53);
      match(ExprParser::T__11);
      break;
    }

    }
    _ctx->stop = _input->LT(-1);
    setState(68);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 2, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        if (!_parseListeners.empty())
          triggerExitRuleEvent();
        previousContext = _localctx;
        setState(66);
        _errHandler->sync(this);
        switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 1, _ctx)) {
        case 1: {
          auto newContext = _tracker.createInstance<MuldivContext>(_tracker.createInstance<ExprContext>(parentContext, parentState));
          _localctx = newContext;
          pushNewRecursionContext(newContext, startState, RuleExpr);
          setState(57);

          if (!(precpred(_ctx, 8))) throw FailedPredicateException(this, "precpred(_ctx, 8)");
          setState(58);
          dynamic_cast<MuldivContext *>(_localctx)->op = _input->LT(1);
          _la = _input->LA(1);
          if (!(_la == ExprParser::T__4

          || _la == ExprParser::T__5)) {
            dynamic_cast<MuldivContext *>(_localctx)->op = _errHandler->recoverInline(this);
          }
          else {
            _errHandler->reportMatch(this);
            consume();
          }
          setState(59);
          expr(9);
          break;
        }

        case 2: {
          auto newContext = _tracker.createInstance<AddsubContext>(_tracker.createInstance<ExprContext>(parentContext, parentState));
          _localctx = newContext;
          pushNewRecursionContext(newContext, startState, RuleExpr);
          setState(60);

          if (!(precpred(_ctx, 7))) throw FailedPredicateException(this, "precpred(_ctx, 7)");
          setState(61);
          dynamic_cast<AddsubContext *>(_localctx)->op = _input->LT(1);
          _la = _input->LA(1);
          if (!(_la == ExprParser::T__1

          || _la == ExprParser::T__6)) {
            dynamic_cast<AddsubContext *>(_localctx)->op = _errHandler->recoverInline(this);
          }
          else {
            _errHandler->reportMatch(this);
            consume();
          }
          setState(62);
          expr(8);
          break;
        }

        case 3: {
          auto newContext = _tracker.createInstance<ComparisonContext>(_tracker.createInstance<ExprContext>(parentContext, parentState));
          _localctx = newContext;
          pushNewRecursionContext(newContext, startState, RuleExpr);
          setState(63);

          if (!(precpred(_ctx, 6))) throw FailedPredicateException(this, "precpred(_ctx, 6)");
          setState(64);
          match(ExprParser::COMPARISON);
          setState(65);
          expr(7);
          break;
        }

        } 
      }
      setState(70);
      _errHandler->sync(this);
      alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 2, _ctx);
    }
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }
  return _localctx;
}

//----------------- AtomContext ------------------------------------------------------------------

ExprParser::AtomContext::AtomContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}


size_t ExprParser::AtomContext::getRuleIndex() const {
  return ExprParser::RuleAtom;
}

void ExprParser::AtomContext::copyFrom(AtomContext *ctx) {
  ParserRuleContext::copyFrom(ctx);
}

//----------------- NumberContext ------------------------------------------------------------------

tree::TerminalNode* ExprParser::NumberContext::NUMBER() {
  return getToken(ExprParser::NUMBER, 0);
}

ExprParser::NumberContext::NumberContext(AtomContext *ctx) { copyFrom(ctx); }

antlrcpp::Any ExprParser::NumberContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<ExprVisitor*>(visitor))
    return parserVisitor->visitNumber(this);
  else
    return visitor->visitChildren(this);
}
//----------------- IdentifierContext ------------------------------------------------------------------

tree::TerminalNode* ExprParser::IdentifierContext::IDENTIFIER() {
  return getToken(ExprParser::IDENTIFIER, 0);
}

ExprParser::IdentifierContext::IdentifierContext(AtomContext *ctx) { copyFrom(ctx); }

antlrcpp::Any ExprParser::IdentifierContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<ExprVisitor*>(visitor))
    return parserVisitor->visitIdentifier(this);
  else
    return visitor->visitChildren(this);
}
ExprParser::AtomContext* ExprParser::atom() {
  AtomContext *_localctx = _tracker.createInstance<AtomContext>(_ctx, getState());
  enterRule(_localctx, 4, ExprParser::RuleAtom);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    setState(73);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case ExprParser::NUMBER: {
        _localctx = dynamic_cast<AtomContext *>(_tracker.createInstance<ExprParser::NumberContext>(_localctx));
        enterOuterAlt(_localctx, 1);
        setState(71);
        match(ExprParser::NUMBER);
        break;
      }

      case ExprParser::IDENTIFIER: {
        _localctx = dynamic_cast<AtomContext *>(_tracker.createInstance<ExprParser::IdentifierContext>(_localctx));
        enterOuterAlt(_localctx, 2);
        setState(72);
        match(ExprParser::IDENTIFIER);
        break;
      }

    default:
      throw NoViableAltException(this);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ShiftContext ------------------------------------------------------------------

ExprParser::ShiftContext::ShiftContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* ExprParser::ShiftContext::TIME() {
  return getToken(ExprParser::TIME, 0);
}

ExprParser::Shift_exprContext* ExprParser::ShiftContext::shift_expr() {
  return getRuleContext<ExprParser::Shift_exprContext>(0);
}


size_t ExprParser::ShiftContext::getRuleIndex() const {
  return ExprParser::RuleShift;
}

antlrcpp::Any ExprParser::ShiftContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<ExprVisitor*>(visitor))
    return parserVisitor->visitShift(this);
  else
    return visitor->visitChildren(this);
}

ExprParser::ShiftContext* ExprParser::shift() {
  ShiftContext *_localctx = _tracker.createInstance<ShiftContext>(_ctx, getState());
  enterRule(_localctx, 6, ExprParser::RuleShift);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(75);
    match(ExprParser::TIME);
    setState(77);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == ExprParser::T__1

    || _la == ExprParser::T__6) {
      setState(76);
      shift_expr(0);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Shift_exprContext ------------------------------------------------------------------

ExprParser::Shift_exprContext::Shift_exprContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}


size_t ExprParser::Shift_exprContext::getRuleIndex() const {
  return ExprParser::RuleShift_expr;
}

void ExprParser::Shift_exprContext::copyFrom(Shift_exprContext *ctx) {
  ParserRuleContext::copyFrom(ctx);
}

//----------------- SignedAtomContext ------------------------------------------------------------------

ExprParser::AtomContext* ExprParser::SignedAtomContext::atom() {
  return getRuleContext<ExprParser::AtomContext>(0);
}

ExprParser::SignedAtomContext::SignedAtomContext(Shift_exprContext *ctx) { copyFrom(ctx); }

antlrcpp::Any ExprParser::SignedAtomContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<ExprVisitor*>(visitor))
    return parserVisitor->visitSignedAtom(this);
  else
    return visitor->visitChildren(this);
}
//----------------- SignedExpressionContext ------------------------------------------------------------------

ExprParser::ExprContext* ExprParser::SignedExpressionContext::expr() {
  return getRuleContext<ExprParser::ExprContext>(0);
}

ExprParser::SignedExpressionContext::SignedExpressionContext(Shift_exprContext *ctx) { copyFrom(ctx); }

antlrcpp::Any ExprParser::SignedExpressionContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<ExprVisitor*>(visitor))
    return parserVisitor->visitSignedExpression(this);
  else
    return visitor->visitChildren(this);
}
//----------------- ShiftMuldivContext ------------------------------------------------------------------

ExprParser::Shift_exprContext* ExprParser::ShiftMuldivContext::shift_expr() {
  return getRuleContext<ExprParser::Shift_exprContext>(0);
}

ExprParser::Right_exprContext* ExprParser::ShiftMuldivContext::right_expr() {
  return getRuleContext<ExprParser::Right_exprContext>(0);
}

ExprParser::ShiftMuldivContext::ShiftMuldivContext(Shift_exprContext *ctx) { copyFrom(ctx); }

antlrcpp::Any ExprParser::ShiftMuldivContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<ExprVisitor*>(visitor))
    return parserVisitor->visitShiftMuldiv(this);
  else
    return visitor->visitChildren(this);
}
//----------------- ShiftAddsubContext ------------------------------------------------------------------

ExprParser::Shift_exprContext* ExprParser::ShiftAddsubContext::shift_expr() {
  return getRuleContext<ExprParser::Shift_exprContext>(0);
}

ExprParser::Right_exprContext* ExprParser::ShiftAddsubContext::right_expr() {
  return getRuleContext<ExprParser::Right_exprContext>(0);
}

ExprParser::ShiftAddsubContext::ShiftAddsubContext(Shift_exprContext *ctx) { copyFrom(ctx); }

antlrcpp::Any ExprParser::ShiftAddsubContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<ExprVisitor*>(visitor))
    return parserVisitor->visitShiftAddsub(this);
  else
    return visitor->visitChildren(this);
}

ExprParser::Shift_exprContext* ExprParser::shift_expr() {
   return shift_expr(0);
}

ExprParser::Shift_exprContext* ExprParser::shift_expr(int precedence) {
  ParserRuleContext *parentContext = _ctx;
  size_t parentState = getState();
  ExprParser::Shift_exprContext *_localctx = _tracker.createInstance<Shift_exprContext>(_ctx, parentState);
  ExprParser::Shift_exprContext *previousContext = _localctx;
  (void)previousContext; // Silence compiler, in case the context is not used by generated code.
  size_t startState = 8;
  enterRecursionRule(_localctx, 8, ExprParser::RuleShift_expr, precedence);

    size_t _la = 0;

  auto onExit = finally([=] {
    unrollRecursionContexts(parentContext);
  });
  try {
    size_t alt;
    enterOuterAlt(_localctx, 1);
    setState(87);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 5, _ctx)) {
    case 1: {
      _localctx = _tracker.createInstance<SignedAtomContext>(_localctx);
      _ctx = _localctx;
      previousContext = _localctx;

      setState(80);
      dynamic_cast<SignedAtomContext *>(_localctx)->op = _input->LT(1);
      _la = _input->LA(1);
      if (!(_la == ExprParser::T__1

      || _la == ExprParser::T__6)) {
        dynamic_cast<SignedAtomContext *>(_localctx)->op = _errHandler->recoverInline(this);
      }
      else {
        _errHandler->reportMatch(this);
        consume();
      }
      setState(81);
      atom();
      break;
    }

    case 2: {
      _localctx = _tracker.createInstance<SignedExpressionContext>(_localctx);
      _ctx = _localctx;
      previousContext = _localctx;
      setState(82);
      dynamic_cast<SignedExpressionContext *>(_localctx)->op = _input->LT(1);
      _la = _input->LA(1);
      if (!(_la == ExprParser::T__1

      || _la == ExprParser::T__6)) {
        dynamic_cast<SignedExpressionContext *>(_localctx)->op = _errHandler->recoverInline(this);
      }
      else {
        _errHandler->reportMatch(this);
        consume();
      }
      setState(83);
      match(ExprParser::T__2);
      setState(84);
      expr(0);
      setState(85);
      match(ExprParser::T__3);
      break;
    }

    }
    _ctx->stop = _input->LT(-1);
    setState(97);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 7, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        if (!_parseListeners.empty())
          triggerExitRuleEvent();
        previousContext = _localctx;
        setState(95);
        _errHandler->sync(this);
        switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 6, _ctx)) {
        case 1: {
          auto newContext = _tracker.createInstance<ShiftMuldivContext>(_tracker.createInstance<Shift_exprContext>(parentContext, parentState));
          _localctx = newContext;
          pushNewRecursionContext(newContext, startState, RuleShift_expr);
          setState(89);

          if (!(precpred(_ctx, 4))) throw FailedPredicateException(this, "precpred(_ctx, 4)");
          setState(90);
          dynamic_cast<ShiftMuldivContext *>(_localctx)->op = _input->LT(1);
          _la = _input->LA(1);
          if (!(_la == ExprParser::T__4

          || _la == ExprParser::T__5)) {
            dynamic_cast<ShiftMuldivContext *>(_localctx)->op = _errHandler->recoverInline(this);
          }
          else {
            _errHandler->reportMatch(this);
            consume();
          }
          setState(91);
          right_expr(0);
          break;
        }

        case 2: {
          auto newContext = _tracker.createInstance<ShiftAddsubContext>(_tracker.createInstance<Shift_exprContext>(parentContext, parentState));
          _localctx = newContext;
          pushNewRecursionContext(newContext, startState, RuleShift_expr);
          setState(92);

          if (!(precpred(_ctx, 3))) throw FailedPredicateException(this, "precpred(_ctx, 3)");
          setState(93);
          dynamic_cast<ShiftAddsubContext *>(_localctx)->op = _input->LT(1);
          _la = _input->LA(1);
          if (!(_la == ExprParser::T__1

          || _la == ExprParser::T__6)) {
            dynamic_cast<ShiftAddsubContext *>(_localctx)->op = _errHandler->recoverInline(this);
          }
          else {
            _errHandler->reportMatch(this);
            consume();
          }
          setState(94);
          right_expr(0);
          break;
        }

        } 
      }
      setState(99);
      _errHandler->sync(this);
      alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 7, _ctx);
    }
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }
  return _localctx;
}

//----------------- Right_exprContext ------------------------------------------------------------------

ExprParser::Right_exprContext::Right_exprContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}


size_t ExprParser::Right_exprContext::getRuleIndex() const {
  return ExprParser::RuleRight_expr;
}

void ExprParser::Right_exprContext::copyFrom(Right_exprContext *ctx) {
  ParserRuleContext::copyFrom(ctx);
}

//----------------- RightExpressionContext ------------------------------------------------------------------

ExprParser::ExprContext* ExprParser::RightExpressionContext::expr() {
  return getRuleContext<ExprParser::ExprContext>(0);
}

ExprParser::RightExpressionContext::RightExpressionContext(Right_exprContext *ctx) { copyFrom(ctx); }

antlrcpp::Any ExprParser::RightExpressionContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<ExprVisitor*>(visitor))
    return parserVisitor->visitRightExpression(this);
  else
    return visitor->visitChildren(this);
}
//----------------- RightMuldivContext ------------------------------------------------------------------

std::vector<ExprParser::Right_exprContext *> ExprParser::RightMuldivContext::right_expr() {
  return getRuleContexts<ExprParser::Right_exprContext>();
}

ExprParser::Right_exprContext* ExprParser::RightMuldivContext::right_expr(size_t i) {
  return getRuleContext<ExprParser::Right_exprContext>(i);
}

ExprParser::RightMuldivContext::RightMuldivContext(Right_exprContext *ctx) { copyFrom(ctx); }

antlrcpp::Any ExprParser::RightMuldivContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<ExprVisitor*>(visitor))
    return parserVisitor->visitRightMuldiv(this);
  else
    return visitor->visitChildren(this);
}
//----------------- RightAtomContext ------------------------------------------------------------------

ExprParser::AtomContext* ExprParser::RightAtomContext::atom() {
  return getRuleContext<ExprParser::AtomContext>(0);
}

ExprParser::RightAtomContext::RightAtomContext(Right_exprContext *ctx) { copyFrom(ctx); }

antlrcpp::Any ExprParser::RightAtomContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<ExprVisitor*>(visitor))
    return parserVisitor->visitRightAtom(this);
  else
    return visitor->visitChildren(this);
}

ExprParser::Right_exprContext* ExprParser::right_expr() {
   return right_expr(0);
}

ExprParser::Right_exprContext* ExprParser::right_expr(int precedence) {
  ParserRuleContext *parentContext = _ctx;
  size_t parentState = getState();
  ExprParser::Right_exprContext *_localctx = _tracker.createInstance<Right_exprContext>(_ctx, parentState);
  ExprParser::Right_exprContext *previousContext = _localctx;
  (void)previousContext; // Silence compiler, in case the context is not used by generated code.
  size_t startState = 10;
  enterRecursionRule(_localctx, 10, ExprParser::RuleRight_expr, precedence);

    size_t _la = 0;

  auto onExit = finally([=] {
    unrollRecursionContexts(parentContext);
  });
  try {
    size_t alt;
    enterOuterAlt(_localctx, 1);
    setState(106);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case ExprParser::T__2: {
        _localctx = _tracker.createInstance<RightExpressionContext>(_localctx);
        _ctx = _localctx;
        previousContext = _localctx;

        setState(101);
        match(ExprParser::T__2);
        setState(102);
        expr(0);
        setState(103);
        match(ExprParser::T__3);
        break;
      }

      case ExprParser::NUMBER:
      case ExprParser::IDENTIFIER: {
        _localctx = _tracker.createInstance<RightAtomContext>(_localctx);
        _ctx = _localctx;
        previousContext = _localctx;
        setState(105);
        atom();
        break;
      }

    default:
      throw NoViableAltException(this);
    }
    _ctx->stop = _input->LT(-1);
    setState(113);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 9, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        if (!_parseListeners.empty())
          triggerExitRuleEvent();
        previousContext = _localctx;
        auto newContext = _tracker.createInstance<RightMuldivContext>(_tracker.createInstance<Right_exprContext>(parentContext, parentState));
        _localctx = newContext;
        pushNewRecursionContext(newContext, startState, RuleRight_expr);
        setState(108);

        if (!(precpred(_ctx, 3))) throw FailedPredicateException(this, "precpred(_ctx, 3)");
        setState(109);
        dynamic_cast<RightMuldivContext *>(_localctx)->op = _input->LT(1);
        _la = _input->LA(1);
        if (!(_la == ExprParser::T__4

        || _la == ExprParser::T__5)) {
          dynamic_cast<RightMuldivContext *>(_localctx)->op = _errHandler->recoverInline(this);
        }
        else {
          _errHandler->reportMatch(this);
          consume();
        }
        setState(110);
        right_expr(4); 
      }
      setState(115);
      _errHandler->sync(this);
      alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 9, _ctx);
    }
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }
  return _localctx;
}

bool ExprParser::sempred(RuleContext *context, size_t ruleIndex, size_t predicateIndex) {
  switch (ruleIndex) {
    case 1: return exprSempred(dynamic_cast<ExprContext *>(context), predicateIndex);
    case 4: return shift_exprSempred(dynamic_cast<Shift_exprContext *>(context), predicateIndex);
    case 5: return right_exprSempred(dynamic_cast<Right_exprContext *>(context), predicateIndex);

  default:
    break;
  }
  return true;
}

bool ExprParser::exprSempred(ExprContext *_localctx, size_t predicateIndex) {
  switch (predicateIndex) {
    case 0: return precpred(_ctx, 8);
    case 1: return precpred(_ctx, 7);
    case 2: return precpred(_ctx, 6);

  default:
    break;
  }
  return true;
}

bool ExprParser::shift_exprSempred(Shift_exprContext *_localctx, size_t predicateIndex) {
  switch (predicateIndex) {
    case 3: return precpred(_ctx, 4);
    case 4: return precpred(_ctx, 3);

  default:
    break;
  }
  return true;
}

bool ExprParser::right_exprSempred(Right_exprContext *_localctx, size_t predicateIndex) {
  switch (predicateIndex) {
    case 5: return precpred(_ctx, 3);

  default:
    break;
  }
  return true;
}

// Static vars and initialization.
std::vector<dfa::DFA> ExprParser::_decisionToDFA;
atn::PredictionContextCache ExprParser::_sharedContextCache;

// We own the ATN which in turn owns the ATN states.
atn::ATN ExprParser::_atn;
std::vector<uint16_t> ExprParser::_serializedATN;

std::vector<std::string> ExprParser::_ruleNames = {
  "fullexpr", "expr", "atom", "shift", "shift_expr", "right_expr"
};

std::vector<std::string> ExprParser::_literalNames = {
  "", "'.'", "'-'", "'('", "')'", "'/'", "'*'", "'+'", "'sum'", "'..'", 
  "','", "'['", "']'", "", "'t'"
};

std::vector<std::string> ExprParser::_symbolicNames = {
  "", "", "", "", "", "", "", "", "", "", "", "", "", "NUMBER", "TIME", 
  "IDENTIFIER", "COMPARISON", "WS"
};

dfa::Vocabulary ExprParser::_vocabulary(_literalNames, _symbolicNames);

std::vector<std::string> ExprParser::_tokenNames;

ExprParser::Initializer::Initializer() {
	for (size_t i = 0; i < _symbolicNames.size(); ++i) {
		std::string name = _vocabulary.getLiteralName(i);
		if (name.empty()) {
			name = _vocabulary.getSymbolicName(i);
		}

		if (name.empty()) {
			_tokenNames.push_back("<INVALID>");
		} else {
      _tokenNames.push_back(name);
    }
	}

  _serializedATN = {
    0x3, 0x608b, 0xa72a, 0x8133, 0xb9ed, 0x417c, 0x3be7, 0x7786, 0x5964, 
    0x3, 0x13, 0x77, 0x4, 0x2, 0x9, 0x2, 0x4, 0x3, 0x9, 0x3, 0x4, 0x4, 0x9, 
    0x4, 0x4, 0x5, 0x9, 0x5, 0x4, 0x6, 0x9, 0x6, 0x4, 0x7, 0x9, 0x7, 0x3, 
    0x2, 0x3, 0x2, 0x3, 0x2, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 
    0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 
    0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 
    0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 
    0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 
    0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 
    0x3, 0x5, 0x3, 0x3a, 0xa, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 
    0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x7, 0x3, 0x45, 0xa, 
    0x3, 0xc, 0x3, 0xe, 0x3, 0x48, 0xb, 0x3, 0x3, 0x4, 0x3, 0x4, 0x5, 0x4, 
    0x4c, 0xa, 0x4, 0x3, 0x5, 0x3, 0x5, 0x5, 0x5, 0x50, 0xa, 0x5, 0x3, 0x6, 
    0x3, 0x6, 0x3, 0x6, 0x3, 0x6, 0x3, 0x6, 0x3, 0x6, 0x3, 0x6, 0x3, 0x6, 
    0x5, 0x6, 0x5a, 0xa, 0x6, 0x3, 0x6, 0x3, 0x6, 0x3, 0x6, 0x3, 0x6, 0x3, 
    0x6, 0x3, 0x6, 0x7, 0x6, 0x62, 0xa, 0x6, 0xc, 0x6, 0xe, 0x6, 0x65, 0xb, 
    0x6, 0x3, 0x7, 0x3, 0x7, 0x3, 0x7, 0x3, 0x7, 0x3, 0x7, 0x3, 0x7, 0x5, 
    0x7, 0x6d, 0xa, 0x7, 0x3, 0x7, 0x3, 0x7, 0x3, 0x7, 0x7, 0x7, 0x72, 0xa, 
    0x7, 0xc, 0x7, 0xe, 0x7, 0x75, 0xb, 0x7, 0x3, 0x7, 0x2, 0x5, 0x4, 0xa, 
    0xc, 0x8, 0x2, 0x4, 0x6, 0x8, 0xa, 0xc, 0x2, 0x4, 0x3, 0x2, 0x7, 0x8, 
    0x4, 0x2, 0x4, 0x4, 0x9, 0x9, 0x2, 0x82, 0x2, 0xe, 0x3, 0x2, 0x2, 0x2, 
    0x4, 0x39, 0x3, 0x2, 0x2, 0x2, 0x6, 0x4b, 0x3, 0x2, 0x2, 0x2, 0x8, 0x4d, 
    0x3, 0x2, 0x2, 0x2, 0xa, 0x59, 0x3, 0x2, 0x2, 0x2, 0xc, 0x6c, 0x3, 0x2, 
    0x2, 0x2, 0xe, 0xf, 0x5, 0x4, 0x3, 0x2, 0xf, 0x10, 0x7, 0x2, 0x2, 0x3, 
    0x10, 0x3, 0x3, 0x2, 0x2, 0x2, 0x11, 0x12, 0x8, 0x3, 0x1, 0x2, 0x12, 
    0x3a, 0x5, 0x6, 0x4, 0x2, 0x13, 0x14, 0x7, 0x11, 0x2, 0x2, 0x14, 0x15, 
    0x7, 0x3, 0x2, 0x2, 0x15, 0x3a, 0x7, 0x11, 0x2, 0x2, 0x16, 0x17, 0x7, 
    0x4, 0x2, 0x2, 0x17, 0x3a, 0x5, 0x4, 0x3, 0xc, 0x18, 0x19, 0x7, 0x5, 
    0x2, 0x2, 0x19, 0x1a, 0x5, 0x4, 0x3, 0x2, 0x1a, 0x1b, 0x7, 0x6, 0x2, 
    0x2, 0x1b, 0x3a, 0x3, 0x2, 0x2, 0x2, 0x1c, 0x1d, 0x7, 0xa, 0x2, 0x2, 
    0x1d, 0x1e, 0x7, 0x5, 0x2, 0x2, 0x1e, 0x1f, 0x5, 0x4, 0x3, 0x2, 0x1f, 
    0x20, 0x7, 0x6, 0x2, 0x2, 0x20, 0x3a, 0x3, 0x2, 0x2, 0x2, 0x21, 0x22, 
    0x7, 0xa, 0x2, 0x2, 0x22, 0x23, 0x7, 0x5, 0x2, 0x2, 0x23, 0x24, 0x5, 
    0x8, 0x5, 0x2, 0x24, 0x25, 0x7, 0xb, 0x2, 0x2, 0x25, 0x26, 0x5, 0x8, 
    0x5, 0x2, 0x26, 0x27, 0x7, 0xc, 0x2, 0x2, 0x27, 0x28, 0x5, 0x4, 0x3, 
    0x2, 0x28, 0x29, 0x7, 0x6, 0x2, 0x2, 0x29, 0x3a, 0x3, 0x2, 0x2, 0x2, 
    0x2a, 0x2b, 0x7, 0x11, 0x2, 0x2, 0x2b, 0x2c, 0x7, 0x5, 0x2, 0x2, 0x2c, 
    0x2d, 0x5, 0x4, 0x3, 0x2, 0x2d, 0x2e, 0x7, 0x6, 0x2, 0x2, 0x2e, 0x3a, 
    0x3, 0x2, 0x2, 0x2, 0x2f, 0x30, 0x7, 0x11, 0x2, 0x2, 0x30, 0x31, 0x7, 
    0xd, 0x2, 0x2, 0x31, 0x32, 0x5, 0x8, 0x5, 0x2, 0x32, 0x33, 0x7, 0xe, 
    0x2, 0x2, 0x33, 0x3a, 0x3, 0x2, 0x2, 0x2, 0x34, 0x35, 0x7, 0x11, 0x2, 
    0x2, 0x35, 0x36, 0x7, 0xd, 0x2, 0x2, 0x36, 0x37, 0x5, 0x4, 0x3, 0x2, 
    0x37, 0x38, 0x7, 0xe, 0x2, 0x2, 0x38, 0x3a, 0x3, 0x2, 0x2, 0x2, 0x39, 
    0x11, 0x3, 0x2, 0x2, 0x2, 0x39, 0x13, 0x3, 0x2, 0x2, 0x2, 0x39, 0x16, 
    0x3, 0x2, 0x2, 0x2, 0x39, 0x18, 0x3, 0x2, 0x2, 0x2, 0x39, 0x1c, 0x3, 
    0x2, 0x2, 0x2, 0x39, 0x21, 0x3, 0x2, 0x2, 0x2, 0x39, 0x2a, 0x3, 0x2, 
    0x2, 0x2, 0x39, 0x2f, 0x3, 0x2, 0x2, 0x2, 0x39, 0x34, 0x3, 0x2, 0x2, 
    0x2, 0x3a, 0x46, 0x3, 0x2, 0x2, 0x2, 0x3b, 0x3c, 0xc, 0xa, 0x2, 0x2, 
    0x3c, 0x3d, 0x9, 0x2, 0x2, 0x2, 0x3d, 0x45, 0x5, 0x4, 0x3, 0xb, 0x3e, 
    0x3f, 0xc, 0x9, 0x2, 0x2, 0x3f, 0x40, 0x9, 0x3, 0x2, 0x2, 0x40, 0x45, 
    0x5, 0x4, 0x3, 0xa, 0x41, 0x42, 0xc, 0x8, 0x2, 0x2, 0x42, 0x43, 0x7, 
    0x12, 0x2, 0x2, 0x43, 0x45, 0x5, 0x4, 0x3, 0x9, 0x44, 0x3b, 0x3, 0x2, 
    0x2, 0x2, 0x44, 0x3e, 0x3, 0x2, 0x2, 0x2, 0x44, 0x41, 0x3, 0x2, 0x2, 
    0x2, 0x45, 0x48, 0x3, 0x2, 0x2, 0x2, 0x46, 0x44, 0x3, 0x2, 0x2, 0x2, 
    0x46, 0x47, 0x3, 0x2, 0x2, 0x2, 0x47, 0x5, 0x3, 0x2, 0x2, 0x2, 0x48, 
    0x46, 0x3, 0x2, 0x2, 0x2, 0x49, 0x4c, 0x7, 0xf, 0x2, 0x2, 0x4a, 0x4c, 
    0x7, 0x11, 0x2, 0x2, 0x4b, 0x49, 0x3, 0x2, 0x2, 0x2, 0x4b, 0x4a, 0x3, 
    0x2, 0x2, 0x2, 0x4c, 0x7, 0x3, 0x2, 0x2, 0x2, 0x4d, 0x4f, 0x7, 0x10, 
    0x2, 0x2, 0x4e, 0x50, 0x5, 0xa, 0x6, 0x2, 0x4f, 0x4e, 0x3, 0x2, 0x2, 
    0x2, 0x4f, 0x50, 0x3, 0x2, 0x2, 0x2, 0x50, 0x9, 0x3, 0x2, 0x2, 0x2, 
    0x51, 0x52, 0x8, 0x6, 0x1, 0x2, 0x52, 0x53, 0x9, 0x3, 0x2, 0x2, 0x53, 
    0x5a, 0x5, 0x6, 0x4, 0x2, 0x54, 0x55, 0x9, 0x3, 0x2, 0x2, 0x55, 0x56, 
    0x7, 0x5, 0x2, 0x2, 0x56, 0x57, 0x5, 0x4, 0x3, 0x2, 0x57, 0x58, 0x7, 
    0x6, 0x2, 0x2, 0x58, 0x5a, 0x3, 0x2, 0x2, 0x2, 0x59, 0x51, 0x3, 0x2, 
    0x2, 0x2, 0x59, 0x54, 0x3, 0x2, 0x2, 0x2, 0x5a, 0x63, 0x3, 0x2, 0x2, 
    0x2, 0x5b, 0x5c, 0xc, 0x6, 0x2, 0x2, 0x5c, 0x5d, 0x9, 0x2, 0x2, 0x2, 
    0x5d, 0x62, 0x5, 0xc, 0x7, 0x2, 0x5e, 0x5f, 0xc, 0x5, 0x2, 0x2, 0x5f, 
    0x60, 0x9, 0x3, 0x2, 0x2, 0x60, 0x62, 0x5, 0xc, 0x7, 0x2, 0x61, 0x5b, 
    0x3, 0x2, 0x2, 0x2, 0x61, 0x5e, 0x3, 0x2, 0x2, 0x2, 0x62, 0x65, 0x3, 
    0x2, 0x2, 0x2, 0x63, 0x61, 0x3, 0x2, 0x2, 0x2, 0x63, 0x64, 0x3, 0x2, 
    0x2, 0x2, 0x64, 0xb, 0x3, 0x2, 0x2, 0x2, 0x65, 0x63, 0x3, 0x2, 0x2, 
    0x2, 0x66, 0x67, 0x8, 0x7, 0x1, 0x2, 0x67, 0x68, 0x7, 0x5, 0x2, 0x2, 
    0x68, 0x69, 0x5, 0x4, 0x3, 0x2, 0x69, 0x6a, 0x7, 0x6, 0x2, 0x2, 0x6a, 
    0x6d, 0x3, 0x2, 0x2, 0x2, 0x6b, 0x6d, 0x5, 0x6, 0x4, 0x2, 0x6c, 0x66, 
    0x3, 0x2, 0x2, 0x2, 0x6c, 0x6b, 0x3, 0x2, 0x2, 0x2, 0x6d, 0x73, 0x3, 
    0x2, 0x2, 0x2, 0x6e, 0x6f, 0xc, 0x5, 0x2, 0x2, 0x6f, 0x70, 0x9, 0x2, 
    0x2, 0x2, 0x70, 0x72, 0x5, 0xc, 0x7, 0x6, 0x71, 0x6e, 0x3, 0x2, 0x2, 
    0x2, 0x72, 0x75, 0x3, 0x2, 0x2, 0x2, 0x73, 0x71, 0x3, 0x2, 0x2, 0x2, 
    0x73, 0x74, 0x3, 0x2, 0x2, 0x2, 0x74, 0xd, 0x3, 0x2, 0x2, 0x2, 0x75, 
    0x73, 0x3, 0x2, 0x2, 0x2, 0xc, 0x39, 0x44, 0x46, 0x4b, 0x4f, 0x59, 0x61, 
    0x63, 0x6c, 0x73, 
  };

  atn::ATNDeserializer deserializer;
  _atn = deserializer.deserialize(_serializedATN);

  size_t count = _atn.getNumberOfDecisions();
  _decisionToDFA.reserve(count);
  for (size_t i = 0; i < count; i++) { 
    _decisionToDFA.emplace_back(_atn.getDecisionState(i), i);
  }
}

ExprParser::Initializer ExprParser::_init;
