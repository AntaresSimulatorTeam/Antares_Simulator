
// Generated from Expr.g4 by ANTLR 4.13.2


#include "ExprVisitor.h"

#include "ExprParser.h"


using namespace antlrcpp;

using namespace antlr4;

namespace {

struct ExprParserStaticData final {
  ExprParserStaticData(std::vector<std::string> ruleNames,
                        std::vector<std::string> literalNames,
                        std::vector<std::string> symbolicNames)
      : ruleNames(std::move(ruleNames)), literalNames(std::move(literalNames)),
        symbolicNames(std::move(symbolicNames)),
        vocabulary(this->literalNames, this->symbolicNames) {}

  ExprParserStaticData(const ExprParserStaticData&) = delete;
  ExprParserStaticData(ExprParserStaticData&&) = delete;
  ExprParserStaticData& operator=(const ExprParserStaticData&) = delete;
  ExprParserStaticData& operator=(ExprParserStaticData&&) = delete;

  std::vector<antlr4::dfa::DFA> decisionToDFA;
  antlr4::atn::PredictionContextCache sharedContextCache;
  const std::vector<std::string> ruleNames;
  const std::vector<std::string> literalNames;
  const std::vector<std::string> symbolicNames;
  const antlr4::dfa::Vocabulary vocabulary;
  antlr4::atn::SerializedATNView serializedATN;
  std::unique_ptr<antlr4::atn::ATN> atn;
};

::antlr4::internal::OnceFlag exprParserOnceFlag;
#if ANTLR4_USE_THREAD_LOCAL_CACHE
static thread_local
#endif
std::unique_ptr<ExprParserStaticData> exprParserStaticData = nullptr;

void exprParserInitialize() {
#if ANTLR4_USE_THREAD_LOCAL_CACHE
  if (exprParserStaticData != nullptr) {
    return;
  }
#else
  assert(exprParserStaticData == nullptr);
#endif
  auto staticData = std::make_unique<ExprParserStaticData>(
    std::vector<std::string>{
      "fullexpr", "expr", "atom", "shift", "shift_expr", "right_expr"
    },
    std::vector<std::string>{
      "", "'.'", "'-'", "'('", "')'", "'/'", "'*'", "'+'", "'sum'", "'..'", 
      "','", "'['", "']'", "", "'t'"
    },
    std::vector<std::string>{
      "", "", "", "", "", "", "", "", "", "", "", "", "", "NUMBER", "TIME", 
      "IDENTIFIER", "COMPARISON", "WS"
    }
  );
  static const int32_t serializedATNSegment[] = {
  	4,1,17,124,2,0,7,0,2,1,7,1,2,2,7,2,2,3,7,3,2,4,7,4,2,5,7,5,1,0,1,0,1,
  	0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,3,1,63,8,1,1,1,1,1,
  	1,1,1,1,1,1,1,1,1,1,1,1,1,1,5,1,74,8,1,10,1,12,1,77,9,1,1,2,1,2,3,2,81,
  	8,2,1,3,1,3,3,3,85,8,3,1,4,1,4,1,4,1,4,1,4,1,4,1,4,1,4,3,4,95,8,4,1,4,
  	1,4,1,4,1,4,1,4,1,4,5,4,103,8,4,10,4,12,4,106,9,4,1,5,1,5,1,5,1,5,1,5,
  	1,5,3,5,114,8,5,1,5,1,5,1,5,5,5,119,8,5,10,5,12,5,122,9,5,1,5,0,3,2,8,
  	10,6,0,2,4,6,8,10,0,2,1,0,5,6,2,0,2,2,7,7,136,0,12,1,0,0,0,2,62,1,0,0,
  	0,4,80,1,0,0,0,6,82,1,0,0,0,8,94,1,0,0,0,10,113,1,0,0,0,12,13,3,2,1,0,
  	13,14,5,0,0,1,14,1,1,0,0,0,15,16,6,1,-1,0,16,63,3,4,2,0,17,18,5,15,0,
  	0,18,19,5,1,0,0,19,63,5,15,0,0,20,21,5,2,0,0,21,63,3,2,1,11,22,23,5,3,
  	0,0,23,24,3,2,1,0,24,25,5,4,0,0,25,63,1,0,0,0,26,27,5,8,0,0,27,28,5,3,
  	0,0,28,29,3,2,1,0,29,30,5,4,0,0,30,63,1,0,0,0,31,32,5,8,0,0,32,33,5,3,
  	0,0,33,34,3,6,3,0,34,35,5,9,0,0,35,36,3,6,3,0,36,37,5,10,0,0,37,38,3,
  	2,1,0,38,39,5,4,0,0,39,63,1,0,0,0,40,41,5,15,0,0,41,42,5,3,0,0,42,43,
  	3,2,1,0,43,44,5,4,0,0,44,63,1,0,0,0,45,46,5,15,0,0,46,47,5,11,0,0,47,
  	48,3,6,3,0,48,49,5,12,0,0,49,63,1,0,0,0,50,51,5,15,0,0,51,52,5,11,0,0,
  	52,53,3,2,1,0,53,54,5,12,0,0,54,63,1,0,0,0,55,56,5,3,0,0,56,57,3,2,1,
  	0,57,58,5,4,0,0,58,59,5,11,0,0,59,60,3,6,3,0,60,61,5,12,0,0,61,63,1,0,
  	0,0,62,15,1,0,0,0,62,17,1,0,0,0,62,20,1,0,0,0,62,22,1,0,0,0,62,26,1,0,
  	0,0,62,31,1,0,0,0,62,40,1,0,0,0,62,45,1,0,0,0,62,50,1,0,0,0,62,55,1,0,
  	0,0,63,75,1,0,0,0,64,65,10,9,0,0,65,66,7,0,0,0,66,74,3,2,1,10,67,68,10,
  	8,0,0,68,69,7,1,0,0,69,74,3,2,1,9,70,71,10,7,0,0,71,72,5,16,0,0,72,74,
  	3,2,1,8,73,64,1,0,0,0,73,67,1,0,0,0,73,70,1,0,0,0,74,77,1,0,0,0,75,73,
  	1,0,0,0,75,76,1,0,0,0,76,3,1,0,0,0,77,75,1,0,0,0,78,81,5,13,0,0,79,81,
  	5,15,0,0,80,78,1,0,0,0,80,79,1,0,0,0,81,5,1,0,0,0,82,84,5,14,0,0,83,85,
  	3,8,4,0,84,83,1,0,0,0,84,85,1,0,0,0,85,7,1,0,0,0,86,87,6,4,-1,0,87,88,
  	7,1,0,0,88,95,3,4,2,0,89,90,7,1,0,0,90,91,5,3,0,0,91,92,3,2,1,0,92,93,
  	5,4,0,0,93,95,1,0,0,0,94,86,1,0,0,0,94,89,1,0,0,0,95,104,1,0,0,0,96,97,
  	10,4,0,0,97,98,7,0,0,0,98,103,3,10,5,0,99,100,10,3,0,0,100,101,7,1,0,
  	0,101,103,3,10,5,0,102,96,1,0,0,0,102,99,1,0,0,0,103,106,1,0,0,0,104,
  	102,1,0,0,0,104,105,1,0,0,0,105,9,1,0,0,0,106,104,1,0,0,0,107,108,6,5,
  	-1,0,108,109,5,3,0,0,109,110,3,2,1,0,110,111,5,4,0,0,111,114,1,0,0,0,
  	112,114,3,4,2,0,113,107,1,0,0,0,113,112,1,0,0,0,114,120,1,0,0,0,115,116,
  	10,3,0,0,116,117,7,0,0,0,117,119,3,10,5,4,118,115,1,0,0,0,119,122,1,0,
  	0,0,120,118,1,0,0,0,120,121,1,0,0,0,121,11,1,0,0,0,122,120,1,0,0,0,10,
  	62,73,75,80,84,94,102,104,113,120
  };
  staticData->serializedATN = antlr4::atn::SerializedATNView(serializedATNSegment, sizeof(serializedATNSegment) / sizeof(serializedATNSegment[0]));

  antlr4::atn::ATNDeserializer deserializer;
  staticData->atn = deserializer.deserialize(staticData->serializedATN);

  const size_t count = staticData->atn->getNumberOfDecisions();
  staticData->decisionToDFA.reserve(count);
  for (size_t i = 0; i < count; i++) { 
    staticData->decisionToDFA.emplace_back(staticData->atn->getDecisionState(i), i);
  }
  exprParserStaticData = std::move(staticData);
}

}

ExprParser::ExprParser(TokenStream *input) : ExprParser(input, antlr4::atn::ParserATNSimulatorOptions()) {}

ExprParser::ExprParser(TokenStream *input, const antlr4::atn::ParserATNSimulatorOptions &options) : Parser(input) {
  ExprParser::initialize();
  _interpreter = new atn::ParserATNSimulator(this, *exprParserStaticData->atn, exprParserStaticData->decisionToDFA, exprParserStaticData->sharedContextCache, options);
}

ExprParser::~ExprParser() {
  delete _interpreter;
}

const atn::ATN& ExprParser::getATN() const {
  return *exprParserStaticData->atn;
}

std::string ExprParser::getGrammarFileName() const {
  return "Expr.g4";
}

const std::vector<std::string>& ExprParser::getRuleNames() const {
  return exprParserStaticData->ruleNames;
}

const dfa::Vocabulary& ExprParser::getVocabulary() const {
  return exprParserStaticData->vocabulary;
}

antlr4::atn::SerializedATNView ExprParser::getSerializedATN() const {
  return exprParserStaticData->serializedATN;
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


std::any ExprParser::FullexprContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<ExprVisitor*>(visitor))
    return parserVisitor->visitFullexpr(this);
  else
    return visitor->visitChildren(this);
}

ExprParser::FullexprContext* ExprParser::fullexpr() {
  FullexprContext *_localctx = _tracker.createInstance<FullexprContext>(_ctx, getState());
  enterRule(_localctx, 0, ExprParser::RuleFullexpr);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
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

//----------------- NegationContext ------------------------------------------------------------------

ExprParser::ExprContext* ExprParser::NegationContext::expr() {
  return getRuleContext<ExprParser::ExprContext>(0);
}

ExprParser::NegationContext::NegationContext(ExprContext *ctx) { copyFrom(ctx); }


std::any ExprParser::NegationContext::accept(tree::ParseTreeVisitor *visitor) {
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


std::any ExprParser::UnsignedAtomContext::accept(tree::ParseTreeVisitor *visitor) {
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


std::any ExprParser::ExpressionContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<ExprVisitor*>(visitor))
    return parserVisitor->visitExpression(this);
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


std::any ExprParser::ComparisonContext::accept(tree::ParseTreeVisitor *visitor) {
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


std::any ExprParser::AllTimeSumContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<ExprVisitor*>(visitor))
    return parserVisitor->visitAllTimeSum(this);
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


std::any ExprParser::AddsubContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<ExprVisitor*>(visitor))
    return parserVisitor->visitAddsub(this);
  else
    return visitor->visitChildren(this);
}
//----------------- TimeShiftExprContext ------------------------------------------------------------------

ExprParser::ExprContext* ExprParser::TimeShiftExprContext::expr() {
  return getRuleContext<ExprParser::ExprContext>(0);
}

ExprParser::ShiftContext* ExprParser::TimeShiftExprContext::shift() {
  return getRuleContext<ExprParser::ShiftContext>(0);
}

ExprParser::TimeShiftExprContext::TimeShiftExprContext(ExprContext *ctx) { copyFrom(ctx); }


std::any ExprParser::TimeShiftExprContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<ExprVisitor*>(visitor))
    return parserVisitor->visitTimeShiftExpr(this);
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


std::any ExprParser::PortFieldContext::accept(tree::ParseTreeVisitor *visitor) {
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


std::any ExprParser::MuldivContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<ExprVisitor*>(visitor))
    return parserVisitor->visitMuldiv(this);
  else
    return visitor->visitChildren(this);
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


std::any ExprParser::TimeSumContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<ExprVisitor*>(visitor))
    return parserVisitor->visitTimeSum(this);
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


std::any ExprParser::TimeIndexContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<ExprVisitor*>(visitor))
    return parserVisitor->visitTimeIndex(this);
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


std::any ExprParser::TimeShiftContext::accept(tree::ParseTreeVisitor *visitor) {
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


std::any ExprParser::FunctionContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<ExprVisitor*>(visitor))
    return parserVisitor->visitFunction(this);
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

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    unrollRecursionContexts(parentContext);
  });
  try {
    size_t alt;
    enterOuterAlt(_localctx, 1);
    setState(62);
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
      expr(11);
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
      antlrcpp::downCast<TimeSumContext *>(_localctx)->from = shift();
      setState(34);
      match(ExprParser::T__8);
      setState(35);
      antlrcpp::downCast<TimeSumContext *>(_localctx)->to = shift();
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

    case 10: {
      _localctx = _tracker.createInstance<TimeShiftExprContext>(_localctx);
      _ctx = _localctx;
      previousContext = _localctx;
      setState(55);
      match(ExprParser::T__2);
      setState(56);
      expr(0);
      setState(57);
      match(ExprParser::T__3);
      setState(58);
      match(ExprParser::T__10);
      setState(59);
      shift();
      setState(60);
      match(ExprParser::T__11);
      break;
    }

    default:
      break;
    }
    _ctx->stop = _input->LT(-1);
    setState(75);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 2, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        if (!_parseListeners.empty())
          triggerExitRuleEvent();
        previousContext = _localctx;
        setState(73);
        _errHandler->sync(this);
        switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 1, _ctx)) {
        case 1: {
          auto newContext = _tracker.createInstance<MuldivContext>(_tracker.createInstance<ExprContext>(parentContext, parentState));
          _localctx = newContext;
          pushNewRecursionContext(newContext, startState, RuleExpr);
          setState(64);

          if (!(precpred(_ctx, 9))) throw FailedPredicateException(this, "precpred(_ctx, 9)");
          setState(65);
          antlrcpp::downCast<MuldivContext *>(_localctx)->op = _input->LT(1);
          _la = _input->LA(1);
          if (!(_la == ExprParser::T__4

          || _la == ExprParser::T__5)) {
            antlrcpp::downCast<MuldivContext *>(_localctx)->op = _errHandler->recoverInline(this);
          }
          else {
            _errHandler->reportMatch(this);
            consume();
          }
          setState(66);
          expr(10);
          break;
        }

        case 2: {
          auto newContext = _tracker.createInstance<AddsubContext>(_tracker.createInstance<ExprContext>(parentContext, parentState));
          _localctx = newContext;
          pushNewRecursionContext(newContext, startState, RuleExpr);
          setState(67);

          if (!(precpred(_ctx, 8))) throw FailedPredicateException(this, "precpred(_ctx, 8)");
          setState(68);
          antlrcpp::downCast<AddsubContext *>(_localctx)->op = _input->LT(1);
          _la = _input->LA(1);
          if (!(_la == ExprParser::T__1

          || _la == ExprParser::T__6)) {
            antlrcpp::downCast<AddsubContext *>(_localctx)->op = _errHandler->recoverInline(this);
          }
          else {
            _errHandler->reportMatch(this);
            consume();
          }
          setState(69);
          expr(9);
          break;
        }

        case 3: {
          auto newContext = _tracker.createInstance<ComparisonContext>(_tracker.createInstance<ExprContext>(parentContext, parentState));
          _localctx = newContext;
          pushNewRecursionContext(newContext, startState, RuleExpr);
          setState(70);

          if (!(precpred(_ctx, 7))) throw FailedPredicateException(this, "precpred(_ctx, 7)");
          setState(71);
          match(ExprParser::COMPARISON);
          setState(72);
          expr(8);
          break;
        }

        default:
          break;
        } 
      }
      setState(77);
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


std::any ExprParser::NumberContext::accept(tree::ParseTreeVisitor *visitor) {
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


std::any ExprParser::IdentifierContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<ExprVisitor*>(visitor))
    return parserVisitor->visitIdentifier(this);
  else
    return visitor->visitChildren(this);
}
ExprParser::AtomContext* ExprParser::atom() {
  AtomContext *_localctx = _tracker.createInstance<AtomContext>(_ctx, getState());
  enterRule(_localctx, 4, ExprParser::RuleAtom);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(80);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case ExprParser::NUMBER: {
        _localctx = _tracker.createInstance<ExprParser::NumberContext>(_localctx);
        enterOuterAlt(_localctx, 1);
        setState(78);
        match(ExprParser::NUMBER);
        break;
      }

      case ExprParser::IDENTIFIER: {
        _localctx = _tracker.createInstance<ExprParser::IdentifierContext>(_localctx);
        enterOuterAlt(_localctx, 2);
        setState(79);
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


std::any ExprParser::ShiftContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<ExprVisitor*>(visitor))
    return parserVisitor->visitShift(this);
  else
    return visitor->visitChildren(this);
}

ExprParser::ShiftContext* ExprParser::shift() {
  ShiftContext *_localctx = _tracker.createInstance<ShiftContext>(_ctx, getState());
  enterRule(_localctx, 6, ExprParser::RuleShift);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(82);
    match(ExprParser::TIME);
    setState(84);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == ExprParser::T__1

    || _la == ExprParser::T__6) {
      setState(83);
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


std::any ExprParser::SignedAtomContext::accept(tree::ParseTreeVisitor *visitor) {
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


std::any ExprParser::SignedExpressionContext::accept(tree::ParseTreeVisitor *visitor) {
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


std::any ExprParser::ShiftMuldivContext::accept(tree::ParseTreeVisitor *visitor) {
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


std::any ExprParser::ShiftAddsubContext::accept(tree::ParseTreeVisitor *visitor) {
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

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    unrollRecursionContexts(parentContext);
  });
  try {
    size_t alt;
    enterOuterAlt(_localctx, 1);
    setState(94);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 5, _ctx)) {
    case 1: {
      _localctx = _tracker.createInstance<SignedAtomContext>(_localctx);
      _ctx = _localctx;
      previousContext = _localctx;

      setState(87);
      antlrcpp::downCast<SignedAtomContext *>(_localctx)->op = _input->LT(1);
      _la = _input->LA(1);
      if (!(_la == ExprParser::T__1

      || _la == ExprParser::T__6)) {
        antlrcpp::downCast<SignedAtomContext *>(_localctx)->op = _errHandler->recoverInline(this);
      }
      else {
        _errHandler->reportMatch(this);
        consume();
      }
      setState(88);
      atom();
      break;
    }

    case 2: {
      _localctx = _tracker.createInstance<SignedExpressionContext>(_localctx);
      _ctx = _localctx;
      previousContext = _localctx;
      setState(89);
      antlrcpp::downCast<SignedExpressionContext *>(_localctx)->op = _input->LT(1);
      _la = _input->LA(1);
      if (!(_la == ExprParser::T__1

      || _la == ExprParser::T__6)) {
        antlrcpp::downCast<SignedExpressionContext *>(_localctx)->op = _errHandler->recoverInline(this);
      }
      else {
        _errHandler->reportMatch(this);
        consume();
      }
      setState(90);
      match(ExprParser::T__2);
      setState(91);
      expr(0);
      setState(92);
      match(ExprParser::T__3);
      break;
    }

    default:
      break;
    }
    _ctx->stop = _input->LT(-1);
    setState(104);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 7, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        if (!_parseListeners.empty())
          triggerExitRuleEvent();
        previousContext = _localctx;
        setState(102);
        _errHandler->sync(this);
        switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 6, _ctx)) {
        case 1: {
          auto newContext = _tracker.createInstance<ShiftMuldivContext>(_tracker.createInstance<Shift_exprContext>(parentContext, parentState));
          _localctx = newContext;
          pushNewRecursionContext(newContext, startState, RuleShift_expr);
          setState(96);

          if (!(precpred(_ctx, 4))) throw FailedPredicateException(this, "precpred(_ctx, 4)");
          setState(97);
          antlrcpp::downCast<ShiftMuldivContext *>(_localctx)->op = _input->LT(1);
          _la = _input->LA(1);
          if (!(_la == ExprParser::T__4

          || _la == ExprParser::T__5)) {
            antlrcpp::downCast<ShiftMuldivContext *>(_localctx)->op = _errHandler->recoverInline(this);
          }
          else {
            _errHandler->reportMatch(this);
            consume();
          }
          setState(98);
          right_expr(0);
          break;
        }

        case 2: {
          auto newContext = _tracker.createInstance<ShiftAddsubContext>(_tracker.createInstance<Shift_exprContext>(parentContext, parentState));
          _localctx = newContext;
          pushNewRecursionContext(newContext, startState, RuleShift_expr);
          setState(99);

          if (!(precpred(_ctx, 3))) throw FailedPredicateException(this, "precpred(_ctx, 3)");
          setState(100);
          antlrcpp::downCast<ShiftAddsubContext *>(_localctx)->op = _input->LT(1);
          _la = _input->LA(1);
          if (!(_la == ExprParser::T__1

          || _la == ExprParser::T__6)) {
            antlrcpp::downCast<ShiftAddsubContext *>(_localctx)->op = _errHandler->recoverInline(this);
          }
          else {
            _errHandler->reportMatch(this);
            consume();
          }
          setState(101);
          right_expr(0);
          break;
        }

        default:
          break;
        } 
      }
      setState(106);
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


std::any ExprParser::RightExpressionContext::accept(tree::ParseTreeVisitor *visitor) {
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


std::any ExprParser::RightMuldivContext::accept(tree::ParseTreeVisitor *visitor) {
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


std::any ExprParser::RightAtomContext::accept(tree::ParseTreeVisitor *visitor) {
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

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    unrollRecursionContexts(parentContext);
  });
  try {
    size_t alt;
    enterOuterAlt(_localctx, 1);
    setState(113);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case ExprParser::T__2: {
        _localctx = _tracker.createInstance<RightExpressionContext>(_localctx);
        _ctx = _localctx;
        previousContext = _localctx;

        setState(108);
        match(ExprParser::T__2);
        setState(109);
        expr(0);
        setState(110);
        match(ExprParser::T__3);
        break;
      }

      case ExprParser::NUMBER:
      case ExprParser::IDENTIFIER: {
        _localctx = _tracker.createInstance<RightAtomContext>(_localctx);
        _ctx = _localctx;
        previousContext = _localctx;
        setState(112);
        atom();
        break;
      }

    default:
      throw NoViableAltException(this);
    }
    _ctx->stop = _input->LT(-1);
    setState(120);
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
        setState(115);

        if (!(precpred(_ctx, 3))) throw FailedPredicateException(this, "precpred(_ctx, 3)");
        setState(116);
        antlrcpp::downCast<RightMuldivContext *>(_localctx)->op = _input->LT(1);
        _la = _input->LA(1);
        if (!(_la == ExprParser::T__4

        || _la == ExprParser::T__5)) {
          antlrcpp::downCast<RightMuldivContext *>(_localctx)->op = _errHandler->recoverInline(this);
        }
        else {
          _errHandler->reportMatch(this);
          consume();
        }
        setState(117);
        right_expr(4); 
      }
      setState(122);
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
    case 1: return exprSempred(antlrcpp::downCast<ExprContext *>(context), predicateIndex);
    case 4: return shift_exprSempred(antlrcpp::downCast<Shift_exprContext *>(context), predicateIndex);
    case 5: return right_exprSempred(antlrcpp::downCast<Right_exprContext *>(context), predicateIndex);

  default:
    break;
  }
  return true;
}

bool ExprParser::exprSempred(ExprContext *_localctx, size_t predicateIndex) {
  switch (predicateIndex) {
    case 0: return precpred(_ctx, 9);
    case 1: return precpred(_ctx, 8);
    case 2: return precpred(_ctx, 7);

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

void ExprParser::initialize() {
#if ANTLR4_USE_THREAD_LOCAL_CACHE
  exprParserInitialize();
#else
  ::antlr4::internal::call_once(exprParserOnceFlag, exprParserInitialize);
#endif
}
