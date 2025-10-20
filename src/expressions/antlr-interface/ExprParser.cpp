
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
      "portFieldExpr", "fullexpr", "expr", "atom", "shift", "shift_expr", 
      "right_expr"
    },
    std::vector<std::string>{
      "", "'.'", "'-'", "'('", "')'", "'/'", "'*'", "'+'", "'sum'", "'sum_connections'", 
      "'..'", "','", "'dual'", "'reduced_cost'", "'['", "']'", "", "'t'"
    },
    std::vector<std::string>{
      "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "NUMBER", 
      "TIME", "IDENTIFIER", "COMPARISON", "WS"
    }
  );
  static const int32_t serializedATNSegment[] = {
  	4,1,20,148,2,0,7,0,2,1,7,1,2,2,7,2,2,3,7,3,2,4,7,4,2,5,7,5,2,6,7,6,1,
  	0,1,0,1,0,1,0,1,1,1,1,1,1,1,2,1,2,1,2,1,2,1,2,1,2,1,2,1,2,1,2,1,2,1,2,
  	1,2,1,2,1,2,1,2,1,2,1,2,1,2,1,2,1,2,1,2,1,2,1,2,1,2,1,2,1,2,1,2,1,2,1,
  	2,1,2,1,2,1,2,1,2,1,2,1,2,1,2,1,2,1,2,1,2,1,2,1,2,1,2,1,2,1,2,1,2,1,2,
  	1,2,1,2,1,2,1,2,1,2,1,2,1,2,1,2,1,2,1,2,1,2,1,2,1,2,1,2,1,2,1,2,1,2,1,
  	2,1,2,3,2,87,8,2,1,2,1,2,1,2,1,2,1,2,1,2,1,2,1,2,1,2,5,2,98,8,2,10,2,
  	12,2,101,9,2,1,3,1,3,3,3,105,8,3,1,4,1,4,3,4,109,8,4,1,5,1,5,1,5,1,5,
  	1,5,1,5,1,5,1,5,3,5,119,8,5,1,5,1,5,1,5,1,5,1,5,1,5,5,5,127,8,5,10,5,
  	12,5,130,9,5,1,6,1,6,1,6,1,6,1,6,1,6,3,6,138,8,6,1,6,1,6,1,6,5,6,143,
  	8,6,10,6,12,6,146,9,6,1,6,0,3,4,10,12,7,0,2,4,6,8,10,12,0,2,1,0,5,6,2,
  	0,2,2,7,7,163,0,14,1,0,0,0,2,18,1,0,0,0,4,86,1,0,0,0,6,104,1,0,0,0,8,
  	106,1,0,0,0,10,118,1,0,0,0,12,137,1,0,0,0,14,15,5,18,0,0,15,16,5,1,0,
  	0,16,17,5,18,0,0,17,1,1,0,0,0,18,19,3,4,2,0,19,20,5,0,0,1,20,3,1,0,0,
  	0,21,22,6,2,-1,0,22,87,3,6,3,0,23,87,3,0,0,0,24,25,5,2,0,0,25,87,3,4,
  	2,15,26,27,5,3,0,0,27,28,3,4,2,0,28,29,5,4,0,0,29,87,1,0,0,0,30,31,5,
  	8,0,0,31,32,5,3,0,0,32,33,3,4,2,0,33,34,5,4,0,0,34,87,1,0,0,0,35,36,5,
  	9,0,0,36,37,5,3,0,0,37,38,3,0,0,0,38,39,5,4,0,0,39,87,1,0,0,0,40,41,5,
  	8,0,0,41,42,5,3,0,0,42,43,3,8,4,0,43,44,5,10,0,0,44,45,3,8,4,0,45,46,
  	5,11,0,0,46,47,3,4,2,0,47,48,5,4,0,0,48,87,1,0,0,0,49,50,5,12,0,0,50,
  	51,5,3,0,0,51,52,5,18,0,0,52,87,5,4,0,0,53,54,5,13,0,0,54,55,5,3,0,0,
  	55,56,5,18,0,0,56,87,5,4,0,0,57,58,5,18,0,0,58,59,5,3,0,0,59,60,3,4,2,
  	0,60,61,5,4,0,0,61,87,1,0,0,0,62,63,5,18,0,0,63,64,5,14,0,0,64,65,3,8,
  	4,0,65,66,5,15,0,0,66,87,1,0,0,0,67,68,5,18,0,0,68,69,5,14,0,0,69,70,
  	3,4,2,0,70,71,5,15,0,0,71,87,1,0,0,0,72,73,5,3,0,0,73,74,3,4,2,0,74,75,
  	5,4,0,0,75,76,5,14,0,0,76,77,3,8,4,0,77,78,5,15,0,0,78,87,1,0,0,0,79,
  	80,5,3,0,0,80,81,3,4,2,0,81,82,5,4,0,0,82,83,5,14,0,0,83,84,3,4,2,0,84,
  	85,5,15,0,0,85,87,1,0,0,0,86,21,1,0,0,0,86,23,1,0,0,0,86,24,1,0,0,0,86,
  	26,1,0,0,0,86,30,1,0,0,0,86,35,1,0,0,0,86,40,1,0,0,0,86,49,1,0,0,0,86,
  	53,1,0,0,0,86,57,1,0,0,0,86,62,1,0,0,0,86,67,1,0,0,0,86,72,1,0,0,0,86,
  	79,1,0,0,0,87,99,1,0,0,0,88,89,10,13,0,0,89,90,7,0,0,0,90,98,3,4,2,14,
  	91,92,10,12,0,0,92,93,7,1,0,0,93,98,3,4,2,13,94,95,10,11,0,0,95,96,5,
  	19,0,0,96,98,3,4,2,12,97,88,1,0,0,0,97,91,1,0,0,0,97,94,1,0,0,0,98,101,
  	1,0,0,0,99,97,1,0,0,0,99,100,1,0,0,0,100,5,1,0,0,0,101,99,1,0,0,0,102,
  	105,5,16,0,0,103,105,5,18,0,0,104,102,1,0,0,0,104,103,1,0,0,0,105,7,1,
  	0,0,0,106,108,5,17,0,0,107,109,3,10,5,0,108,107,1,0,0,0,108,109,1,0,0,
  	0,109,9,1,0,0,0,110,111,6,5,-1,0,111,112,7,1,0,0,112,119,3,6,3,0,113,
  	114,7,1,0,0,114,115,5,3,0,0,115,116,3,4,2,0,116,117,5,4,0,0,117,119,1,
  	0,0,0,118,110,1,0,0,0,118,113,1,0,0,0,119,128,1,0,0,0,120,121,10,4,0,
  	0,121,122,7,0,0,0,122,127,3,12,6,0,123,124,10,3,0,0,124,125,7,1,0,0,125,
  	127,3,12,6,0,126,120,1,0,0,0,126,123,1,0,0,0,127,130,1,0,0,0,128,126,
  	1,0,0,0,128,129,1,0,0,0,129,11,1,0,0,0,130,128,1,0,0,0,131,132,6,6,-1,
  	0,132,133,5,3,0,0,133,134,3,4,2,0,134,135,5,4,0,0,135,138,1,0,0,0,136,
  	138,3,6,3,0,137,131,1,0,0,0,137,136,1,0,0,0,138,144,1,0,0,0,139,140,10,
  	3,0,0,140,141,7,0,0,0,141,143,3,12,6,4,142,139,1,0,0,0,143,146,1,0,0,
  	0,144,142,1,0,0,0,144,145,1,0,0,0,145,13,1,0,0,0,146,144,1,0,0,0,10,86,
  	97,99,104,108,118,126,128,137,144
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


//----------------- PortFieldExprContext ------------------------------------------------------------------

ExprParser::PortFieldExprContext::PortFieldExprContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<tree::TerminalNode *> ExprParser::PortFieldExprContext::IDENTIFIER() {
  return getTokens(ExprParser::IDENTIFIER);
}

tree::TerminalNode* ExprParser::PortFieldExprContext::IDENTIFIER(size_t i) {
  return getToken(ExprParser::IDENTIFIER, i);
}


size_t ExprParser::PortFieldExprContext::getRuleIndex() const {
  return ExprParser::RulePortFieldExpr;
}


std::any ExprParser::PortFieldExprContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<ExprVisitor*>(visitor))
    return parserVisitor->visitPortFieldExpr(this);
  else
    return visitor->visitChildren(this);
}

ExprParser::PortFieldExprContext* ExprParser::portFieldExpr() {
  PortFieldExprContext *_localctx = _tracker.createInstance<PortFieldExprContext>(_ctx, getState());
  enterRule(_localctx, 0, ExprParser::RulePortFieldExpr);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(14);
    match(ExprParser::IDENTIFIER);
    setState(15);
    match(ExprParser::T__0);
    setState(16);
    match(ExprParser::IDENTIFIER);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
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
  enterRule(_localctx, 2, ExprParser::RuleFullexpr);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(18);
    expr(0);
    setState(19);
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

//----------------- PortFieldSumContext ------------------------------------------------------------------

ExprParser::PortFieldExprContext* ExprParser::PortFieldSumContext::portFieldExpr() {
  return getRuleContext<ExprParser::PortFieldExprContext>(0);
}

ExprParser::PortFieldSumContext::PortFieldSumContext(ExprContext *ctx) { copyFrom(ctx); }


std::any ExprParser::PortFieldSumContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<ExprVisitor*>(visitor))
    return parserVisitor->visitPortFieldSum(this);
  else
    return visitor->visitChildren(this);
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
//----------------- TimeIndexExprContext ------------------------------------------------------------------

std::vector<ExprParser::ExprContext *> ExprParser::TimeIndexExprContext::expr() {
  return getRuleContexts<ExprParser::ExprContext>();
}

ExprParser::ExprContext* ExprParser::TimeIndexExprContext::expr(size_t i) {
  return getRuleContext<ExprParser::ExprContext>(i);
}

ExprParser::TimeIndexExprContext::TimeIndexExprContext(ExprContext *ctx) { copyFrom(ctx); }


std::any ExprParser::TimeIndexExprContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<ExprVisitor*>(visitor))
    return parserVisitor->visitTimeIndexExpr(this);
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

ExprParser::PortFieldExprContext* ExprParser::PortFieldContext::portFieldExpr() {
  return getRuleContext<ExprParser::PortFieldExprContext>(0);
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
//----------------- ReducedCostContext ------------------------------------------------------------------

tree::TerminalNode* ExprParser::ReducedCostContext::IDENTIFIER() {
  return getToken(ExprParser::IDENTIFIER, 0);
}

ExprParser::ReducedCostContext::ReducedCostContext(ExprContext *ctx) { copyFrom(ctx); }


std::any ExprParser::ReducedCostContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<ExprVisitor*>(visitor))
    return parserVisitor->visitReducedCost(this);
  else
    return visitor->visitChildren(this);
}
//----------------- DualContext ------------------------------------------------------------------

tree::TerminalNode* ExprParser::DualContext::IDENTIFIER() {
  return getToken(ExprParser::IDENTIFIER, 0);
}

ExprParser::DualContext::DualContext(ExprContext *ctx) { copyFrom(ctx); }


std::any ExprParser::DualContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<ExprVisitor*>(visitor))
    return parserVisitor->visitDual(this);
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
  size_t startState = 4;
  enterRecursionRule(_localctx, 4, ExprParser::RuleExpr, precedence);

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
    setState(86);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 0, _ctx)) {
    case 1: {
      _localctx = _tracker.createInstance<UnsignedAtomContext>(_localctx);
      _ctx = _localctx;
      previousContext = _localctx;

      setState(22);
      atom();
      break;
    }

    case 2: {
      _localctx = _tracker.createInstance<PortFieldContext>(_localctx);
      _ctx = _localctx;
      previousContext = _localctx;
      setState(23);
      portFieldExpr();
      break;
    }

    case 3: {
      _localctx = _tracker.createInstance<NegationContext>(_localctx);
      _ctx = _localctx;
      previousContext = _localctx;
      setState(24);
      match(ExprParser::T__1);
      setState(25);
      expr(15);
      break;
    }

    case 4: {
      _localctx = _tracker.createInstance<ExpressionContext>(_localctx);
      _ctx = _localctx;
      previousContext = _localctx;
      setState(26);
      match(ExprParser::T__2);
      setState(27);
      expr(0);
      setState(28);
      match(ExprParser::T__3);
      break;
    }

    case 5: {
      _localctx = _tracker.createInstance<AllTimeSumContext>(_localctx);
      _ctx = _localctx;
      previousContext = _localctx;
      setState(30);
      match(ExprParser::T__7);
      setState(31);
      match(ExprParser::T__2);
      setState(32);
      expr(0);
      setState(33);
      match(ExprParser::T__3);
      break;
    }

    case 6: {
      _localctx = _tracker.createInstance<PortFieldSumContext>(_localctx);
      _ctx = _localctx;
      previousContext = _localctx;
      setState(35);
      match(ExprParser::T__8);
      setState(36);
      match(ExprParser::T__2);
      setState(37);
      portFieldExpr();
      setState(38);
      match(ExprParser::T__3);
      break;
    }

    case 7: {
      _localctx = _tracker.createInstance<TimeSumContext>(_localctx);
      _ctx = _localctx;
      previousContext = _localctx;
      setState(40);
      match(ExprParser::T__7);
      setState(41);
      match(ExprParser::T__2);
      setState(42);
      antlrcpp::downCast<TimeSumContext *>(_localctx)->from = shift();
      setState(43);
      match(ExprParser::T__9);
      setState(44);
      antlrcpp::downCast<TimeSumContext *>(_localctx)->to = shift();
      setState(45);
      match(ExprParser::T__10);
      setState(46);
      expr(0);
      setState(47);
      match(ExprParser::T__3);
      break;
    }

    case 8: {
      _localctx = _tracker.createInstance<DualContext>(_localctx);
      _ctx = _localctx;
      previousContext = _localctx;
      setState(49);
      match(ExprParser::T__11);
      setState(50);
      match(ExprParser::T__2);
      setState(51);
      match(ExprParser::IDENTIFIER);
      setState(52);
      match(ExprParser::T__3);
      break;
    }

    case 9: {
      _localctx = _tracker.createInstance<ReducedCostContext>(_localctx);
      _ctx = _localctx;
      previousContext = _localctx;
      setState(53);
      match(ExprParser::T__12);
      setState(54);
      match(ExprParser::T__2);
      setState(55);
      match(ExprParser::IDENTIFIER);
      setState(56);
      match(ExprParser::T__3);
      break;
    }

    case 10: {
      _localctx = _tracker.createInstance<FunctionContext>(_localctx);
      _ctx = _localctx;
      previousContext = _localctx;
      setState(57);
      match(ExprParser::IDENTIFIER);
      setState(58);
      match(ExprParser::T__2);
      setState(59);
      expr(0);
      setState(60);
      match(ExprParser::T__3);
      break;
    }

    case 11: {
      _localctx = _tracker.createInstance<TimeShiftContext>(_localctx);
      _ctx = _localctx;
      previousContext = _localctx;
      setState(62);
      match(ExprParser::IDENTIFIER);
      setState(63);
      match(ExprParser::T__13);
      setState(64);
      shift();
      setState(65);
      match(ExprParser::T__14);
      break;
    }

    case 12: {
      _localctx = _tracker.createInstance<TimeIndexContext>(_localctx);
      _ctx = _localctx;
      previousContext = _localctx;
      setState(67);
      match(ExprParser::IDENTIFIER);
      setState(68);
      match(ExprParser::T__13);
      setState(69);
      expr(0);
      setState(70);
      match(ExprParser::T__14);
      break;
    }

    case 13: {
      _localctx = _tracker.createInstance<TimeShiftExprContext>(_localctx);
      _ctx = _localctx;
      previousContext = _localctx;
      setState(72);
      match(ExprParser::T__2);
      setState(73);
      expr(0);
      setState(74);
      match(ExprParser::T__3);
      setState(75);
      match(ExprParser::T__13);
      setState(76);
      shift();
      setState(77);
      match(ExprParser::T__14);
      break;
    }

    case 14: {
      _localctx = _tracker.createInstance<TimeIndexExprContext>(_localctx);
      _ctx = _localctx;
      previousContext = _localctx;
      setState(79);
      match(ExprParser::T__2);
      setState(80);
      expr(0);
      setState(81);
      match(ExprParser::T__3);
      setState(82);
      match(ExprParser::T__13);
      setState(83);
      expr(0);
      setState(84);
      match(ExprParser::T__14);
      break;
    }

    default:
      break;
    }
    _ctx->stop = _input->LT(-1);
    setState(99);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 2, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        if (!_parseListeners.empty())
          triggerExitRuleEvent();
        previousContext = _localctx;
        setState(97);
        _errHandler->sync(this);
        switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 1, _ctx)) {
        case 1: {
          auto newContext = _tracker.createInstance<MuldivContext>(_tracker.createInstance<ExprContext>(parentContext, parentState));
          _localctx = newContext;
          pushNewRecursionContext(newContext, startState, RuleExpr);
          setState(88);

          if (!(precpred(_ctx, 13))) throw FailedPredicateException(this, "precpred(_ctx, 13)");
          setState(89);
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
          setState(90);
          expr(14);
          break;
        }

        case 2: {
          auto newContext = _tracker.createInstance<AddsubContext>(_tracker.createInstance<ExprContext>(parentContext, parentState));
          _localctx = newContext;
          pushNewRecursionContext(newContext, startState, RuleExpr);
          setState(91);

          if (!(precpred(_ctx, 12))) throw FailedPredicateException(this, "precpred(_ctx, 12)");
          setState(92);
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
          setState(93);
          expr(13);
          break;
        }

        case 3: {
          auto newContext = _tracker.createInstance<ComparisonContext>(_tracker.createInstance<ExprContext>(parentContext, parentState));
          _localctx = newContext;
          pushNewRecursionContext(newContext, startState, RuleExpr);
          setState(94);

          if (!(precpred(_ctx, 11))) throw FailedPredicateException(this, "precpred(_ctx, 11)");
          setState(95);
          match(ExprParser::COMPARISON);
          setState(96);
          expr(12);
          break;
        }

        default:
          break;
        } 
      }
      setState(101);
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
  enterRule(_localctx, 6, ExprParser::RuleAtom);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(104);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case ExprParser::NUMBER: {
        _localctx = _tracker.createInstance<ExprParser::NumberContext>(_localctx);
        enterOuterAlt(_localctx, 1);
        setState(102);
        match(ExprParser::NUMBER);
        break;
      }

      case ExprParser::IDENTIFIER: {
        _localctx = _tracker.createInstance<ExprParser::IdentifierContext>(_localctx);
        enterOuterAlt(_localctx, 2);
        setState(103);
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
  enterRule(_localctx, 8, ExprParser::RuleShift);
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
    setState(106);
    match(ExprParser::TIME);
    setState(108);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == ExprParser::T__1

    || _la == ExprParser::T__6) {
      setState(107);
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
  size_t startState = 10;
  enterRecursionRule(_localctx, 10, ExprParser::RuleShift_expr, precedence);

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
    setState(118);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 5, _ctx)) {
    case 1: {
      _localctx = _tracker.createInstance<SignedAtomContext>(_localctx);
      _ctx = _localctx;
      previousContext = _localctx;

      setState(111);
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
      setState(112);
      atom();
      break;
    }

    case 2: {
      _localctx = _tracker.createInstance<SignedExpressionContext>(_localctx);
      _ctx = _localctx;
      previousContext = _localctx;
      setState(113);
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
      setState(114);
      match(ExprParser::T__2);
      setState(115);
      expr(0);
      setState(116);
      match(ExprParser::T__3);
      break;
    }

    default:
      break;
    }
    _ctx->stop = _input->LT(-1);
    setState(128);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 7, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        if (!_parseListeners.empty())
          triggerExitRuleEvent();
        previousContext = _localctx;
        setState(126);
        _errHandler->sync(this);
        switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 6, _ctx)) {
        case 1: {
          auto newContext = _tracker.createInstance<ShiftMuldivContext>(_tracker.createInstance<Shift_exprContext>(parentContext, parentState));
          _localctx = newContext;
          pushNewRecursionContext(newContext, startState, RuleShift_expr);
          setState(120);

          if (!(precpred(_ctx, 4))) throw FailedPredicateException(this, "precpred(_ctx, 4)");
          setState(121);
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
          setState(122);
          right_expr(0);
          break;
        }

        case 2: {
          auto newContext = _tracker.createInstance<ShiftAddsubContext>(_tracker.createInstance<Shift_exprContext>(parentContext, parentState));
          _localctx = newContext;
          pushNewRecursionContext(newContext, startState, RuleShift_expr);
          setState(123);

          if (!(precpred(_ctx, 3))) throw FailedPredicateException(this, "precpred(_ctx, 3)");
          setState(124);
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
          setState(125);
          right_expr(0);
          break;
        }

        default:
          break;
        } 
      }
      setState(130);
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
  size_t startState = 12;
  enterRecursionRule(_localctx, 12, ExprParser::RuleRight_expr, precedence);

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
    setState(137);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case ExprParser::T__2: {
        _localctx = _tracker.createInstance<RightExpressionContext>(_localctx);
        _ctx = _localctx;
        previousContext = _localctx;

        setState(132);
        match(ExprParser::T__2);
        setState(133);
        expr(0);
        setState(134);
        match(ExprParser::T__3);
        break;
      }

      case ExprParser::NUMBER:
      case ExprParser::IDENTIFIER: {
        _localctx = _tracker.createInstance<RightAtomContext>(_localctx);
        _ctx = _localctx;
        previousContext = _localctx;
        setState(136);
        atom();
        break;
      }

    default:
      throw NoViableAltException(this);
    }
    _ctx->stop = _input->LT(-1);
    setState(144);
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
        setState(139);

        if (!(precpred(_ctx, 3))) throw FailedPredicateException(this, "precpred(_ctx, 3)");
        setState(140);
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
        setState(141);
        right_expr(4); 
      }
      setState(146);
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
    case 2: return exprSempred(antlrcpp::downCast<ExprContext *>(context), predicateIndex);
    case 5: return shift_exprSempred(antlrcpp::downCast<Shift_exprContext *>(context), predicateIndex);
    case 6: return right_exprSempred(antlrcpp::downCast<Right_exprContext *>(context), predicateIndex);

  default:
    break;
  }
  return true;
}

bool ExprParser::exprSempred(ExprContext *_localctx, size_t predicateIndex) {
  switch (predicateIndex) {
    case 0: return precpred(_ctx, 13);
    case 1: return precpred(_ctx, 12);
    case 2: return precpred(_ctx, 11);

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
