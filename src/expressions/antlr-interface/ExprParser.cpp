
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
      "portFieldExpr", "fullexpr", "sum_bound", "expr", "argList", "atom", 
      "shift", "shift_expr", "right_expr"
    },
    std::vector<std::string>{
      "", "'.'", "'-'", "'('", "')'", "'^'", "'/'", "'*'", "'+'", "'sum'", 
      "'sum_connections'", "','", "'no_connection_expr'", "'..'", "'['", 
      "']'", "", "'t'"
    },
    std::vector<std::string>{
      "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "NUMBER", 
      "TIME", "IDENTIFIER", "COMPARISON", "WS"
    }
  );
  static const int32_t serializedATNSegment[] = {
  	4,1,20,172,2,0,7,0,2,1,7,1,2,2,7,2,2,3,7,3,2,4,7,4,2,5,7,5,2,6,7,6,2,
  	7,7,7,2,8,7,8,1,0,1,0,1,0,1,0,1,1,1,1,1,1,1,2,1,2,3,2,28,8,2,1,3,1,3,
  	1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,
  	3,1,3,3,3,51,8,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,
  	1,3,3,3,67,8,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,
  	3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,3,3,94,8,3,1,3,1,3,1,3,
  	1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,5,3,108,8,3,10,3,12,3,111,9,3,1,4,
  	1,4,1,4,5,4,116,8,4,10,4,12,4,119,9,4,1,5,1,5,3,5,123,8,5,1,6,1,6,3,6,
  	127,8,6,1,7,1,7,1,7,1,7,1,7,1,7,1,7,1,7,3,7,137,8,7,1,7,1,7,1,7,1,7,1,
  	7,1,7,1,7,1,7,1,7,5,7,148,8,7,10,7,12,7,151,9,7,1,8,1,8,1,8,1,8,1,8,1,
  	8,3,8,159,8,8,1,8,1,8,1,8,1,8,1,8,1,8,5,8,167,8,8,10,8,12,8,170,9,8,1,
  	8,0,3,6,14,16,9,0,2,4,6,8,10,12,14,16,0,2,1,0,6,7,2,0,2,2,8,8,190,0,18,
  	1,0,0,0,2,22,1,0,0,0,4,27,1,0,0,0,6,93,1,0,0,0,8,112,1,0,0,0,10,122,1,
  	0,0,0,12,124,1,0,0,0,14,136,1,0,0,0,16,158,1,0,0,0,18,19,5,18,0,0,19,
  	20,5,1,0,0,20,21,5,18,0,0,21,1,1,0,0,0,22,23,3,6,3,0,23,24,5,0,0,1,24,
  	3,1,0,0,0,25,28,3,6,3,0,26,28,3,12,6,0,27,25,1,0,0,0,27,26,1,0,0,0,28,
  	5,1,0,0,0,29,30,6,3,-1,0,30,94,3,10,5,0,31,94,3,0,0,0,32,33,5,2,0,0,33,
  	94,3,6,3,14,34,35,5,3,0,0,35,36,3,6,3,0,36,37,5,4,0,0,37,94,1,0,0,0,38,
  	39,5,9,0,0,39,40,5,3,0,0,40,41,3,6,3,0,41,42,5,4,0,0,42,94,1,0,0,0,43,
  	44,5,10,0,0,44,45,5,3,0,0,45,50,3,0,0,0,46,47,5,11,0,0,47,48,5,12,0,0,
  	48,49,5,19,0,0,49,51,3,6,3,0,50,46,1,0,0,0,50,51,1,0,0,0,51,52,1,0,0,
  	0,52,53,5,4,0,0,53,94,1,0,0,0,54,55,5,9,0,0,55,56,5,3,0,0,56,57,3,4,2,
  	0,57,58,5,13,0,0,58,59,3,4,2,0,59,60,5,11,0,0,60,61,3,6,3,0,61,62,5,4,
  	0,0,62,94,1,0,0,0,63,64,5,18,0,0,64,66,5,3,0,0,65,67,3,8,4,0,66,65,1,
  	0,0,0,66,67,1,0,0,0,67,68,1,0,0,0,68,94,5,4,0,0,69,70,5,18,0,0,70,71,
  	5,14,0,0,71,72,3,12,6,0,72,73,5,15,0,0,73,94,1,0,0,0,74,75,5,18,0,0,75,
  	76,5,14,0,0,76,77,3,6,3,0,77,78,5,15,0,0,78,94,1,0,0,0,79,80,5,3,0,0,
  	80,81,3,6,3,0,81,82,5,4,0,0,82,83,5,14,0,0,83,84,3,12,6,0,84,85,5,15,
  	0,0,85,94,1,0,0,0,86,87,5,3,0,0,87,88,3,6,3,0,88,89,5,4,0,0,89,90,5,14,
  	0,0,90,91,3,6,3,0,91,92,5,15,0,0,92,94,1,0,0,0,93,29,1,0,0,0,93,31,1,
  	0,0,0,93,32,1,0,0,0,93,34,1,0,0,0,93,38,1,0,0,0,93,43,1,0,0,0,93,54,1,
  	0,0,0,93,63,1,0,0,0,93,69,1,0,0,0,93,74,1,0,0,0,93,79,1,0,0,0,93,86,1,
  	0,0,0,94,109,1,0,0,0,95,96,10,12,0,0,96,97,5,5,0,0,97,108,3,6,3,12,98,
  	99,10,11,0,0,99,100,7,0,0,0,100,108,3,6,3,12,101,102,10,10,0,0,102,103,
  	7,1,0,0,103,108,3,6,3,11,104,105,10,9,0,0,105,106,5,19,0,0,106,108,3,
  	6,3,10,107,95,1,0,0,0,107,98,1,0,0,0,107,101,1,0,0,0,107,104,1,0,0,0,
  	108,111,1,0,0,0,109,107,1,0,0,0,109,110,1,0,0,0,110,7,1,0,0,0,111,109,
  	1,0,0,0,112,117,3,6,3,0,113,114,5,11,0,0,114,116,3,6,3,0,115,113,1,0,
  	0,0,116,119,1,0,0,0,117,115,1,0,0,0,117,118,1,0,0,0,118,9,1,0,0,0,119,
  	117,1,0,0,0,120,123,5,16,0,0,121,123,5,18,0,0,122,120,1,0,0,0,122,121,
  	1,0,0,0,123,11,1,0,0,0,124,126,5,17,0,0,125,127,3,14,7,0,126,125,1,0,
  	0,0,126,127,1,0,0,0,127,13,1,0,0,0,128,129,6,7,-1,0,129,130,7,1,0,0,130,
  	137,3,10,5,0,131,132,7,1,0,0,132,133,5,3,0,0,133,134,3,6,3,0,134,135,
  	5,4,0,0,135,137,1,0,0,0,136,128,1,0,0,0,136,131,1,0,0,0,137,149,1,0,0,
  	0,138,139,10,5,0,0,139,140,5,5,0,0,140,148,3,16,8,0,141,142,10,4,0,0,
  	142,143,7,0,0,0,143,148,3,16,8,0,144,145,10,3,0,0,145,146,7,1,0,0,146,
  	148,3,16,8,0,147,138,1,0,0,0,147,141,1,0,0,0,147,144,1,0,0,0,148,151,
  	1,0,0,0,149,147,1,0,0,0,149,150,1,0,0,0,150,15,1,0,0,0,151,149,1,0,0,
  	0,152,153,6,8,-1,0,153,154,5,3,0,0,154,155,3,6,3,0,155,156,5,4,0,0,156,
  	159,1,0,0,0,157,159,3,10,5,0,158,152,1,0,0,0,158,157,1,0,0,0,159,168,
  	1,0,0,0,160,161,10,4,0,0,161,162,5,5,0,0,162,167,3,16,8,4,163,164,10,
  	3,0,0,164,165,7,0,0,0,165,167,3,16,8,4,166,160,1,0,0,0,166,163,1,0,0,
  	0,167,170,1,0,0,0,168,166,1,0,0,0,168,169,1,0,0,0,169,17,1,0,0,0,170,
  	168,1,0,0,0,15,27,50,66,93,107,109,117,122,126,136,147,149,158,166,168
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
    setState(18);
    match(ExprParser::IDENTIFIER);
    setState(19);
    match(ExprParser::T__0);
    setState(20);
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
    setState(22);
    expr(0);
    setState(23);
    match(ExprParser::EOF);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Sum_boundContext ------------------------------------------------------------------

ExprParser::Sum_boundContext::Sum_boundContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

ExprParser::ExprContext* ExprParser::Sum_boundContext::expr() {
  return getRuleContext<ExprParser::ExprContext>(0);
}

ExprParser::ShiftContext* ExprParser::Sum_boundContext::shift() {
  return getRuleContext<ExprParser::ShiftContext>(0);
}


size_t ExprParser::Sum_boundContext::getRuleIndex() const {
  return ExprParser::RuleSum_bound;
}


std::any ExprParser::Sum_boundContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<ExprVisitor*>(visitor))
    return parserVisitor->visitSum_bound(this);
  else
    return visitor->visitChildren(this);
}

ExprParser::Sum_boundContext* ExprParser::sum_bound() {
  Sum_boundContext *_localctx = _tracker.createInstance<Sum_boundContext>(_ctx, getState());
  enterRule(_localctx, 4, ExprParser::RuleSum_bound);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(27);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case ExprParser::T__1:
      case ExprParser::T__2:
      case ExprParser::T__8:
      case ExprParser::T__9:
      case ExprParser::NUMBER:
      case ExprParser::IDENTIFIER: {
        enterOuterAlt(_localctx, 1);
        setState(25);
        expr(0);
        break;
      }

      case ExprParser::TIME: {
        enterOuterAlt(_localctx, 2);
        setState(26);
        shift();
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

tree::TerminalNode* ExprParser::PortFieldSumContext::COMPARISON() {
  return getToken(ExprParser::COMPARISON, 0);
}

ExprParser::ExprContext* ExprParser::PortFieldSumContext::expr() {
  return getRuleContext<ExprParser::ExprContext>(0);
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

std::vector<ExprParser::Sum_boundContext *> ExprParser::TimeSumContext::sum_bound() {
  return getRuleContexts<ExprParser::Sum_boundContext>();
}

ExprParser::Sum_boundContext* ExprParser::TimeSumContext::sum_bound(size_t i) {
  return getRuleContext<ExprParser::Sum_boundContext>(i);
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

ExprParser::ArgListContext* ExprParser::FunctionContext::argList() {
  return getRuleContext<ExprParser::ArgListContext>(0);
}

ExprParser::FunctionContext::FunctionContext(ExprContext *ctx) { copyFrom(ctx); }


std::any ExprParser::FunctionContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<ExprVisitor*>(visitor))
    return parserVisitor->visitFunction(this);
  else
    return visitor->visitChildren(this);
}
//----------------- PowerContext ------------------------------------------------------------------

std::vector<ExprParser::ExprContext *> ExprParser::PowerContext::expr() {
  return getRuleContexts<ExprParser::ExprContext>();
}

ExprParser::ExprContext* ExprParser::PowerContext::expr(size_t i) {
  return getRuleContext<ExprParser::ExprContext>(i);
}

ExprParser::PowerContext::PowerContext(ExprContext *ctx) { copyFrom(ctx); }


std::any ExprParser::PowerContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<ExprVisitor*>(visitor))
    return parserVisitor->visitPower(this);
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
  size_t startState = 6;
  enterRecursionRule(_localctx, 6, ExprParser::RuleExpr, precedence);

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
    setState(93);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 3, _ctx)) {
    case 1: {
      _localctx = _tracker.createInstance<UnsignedAtomContext>(_localctx);
      _ctx = _localctx;
      previousContext = _localctx;

      setState(30);
      atom();
      break;
    }

    case 2: {
      _localctx = _tracker.createInstance<PortFieldContext>(_localctx);
      _ctx = _localctx;
      previousContext = _localctx;
      setState(31);
      portFieldExpr();
      break;
    }

    case 3: {
      _localctx = _tracker.createInstance<NegationContext>(_localctx);
      _ctx = _localctx;
      previousContext = _localctx;
      setState(32);
      match(ExprParser::T__1);
      setState(33);
      expr(14);
      break;
    }

    case 4: {
      _localctx = _tracker.createInstance<ExpressionContext>(_localctx);
      _ctx = _localctx;
      previousContext = _localctx;
      setState(34);
      match(ExprParser::T__2);
      setState(35);
      expr(0);
      setState(36);
      match(ExprParser::T__3);
      break;
    }

    case 5: {
      _localctx = _tracker.createInstance<AllTimeSumContext>(_localctx);
      _ctx = _localctx;
      previousContext = _localctx;
      setState(38);
      match(ExprParser::T__8);
      setState(39);
      match(ExprParser::T__2);
      setState(40);
      expr(0);
      setState(41);
      match(ExprParser::T__3);
      break;
    }

    case 6: {
      _localctx = _tracker.createInstance<PortFieldSumContext>(_localctx);
      _ctx = _localctx;
      previousContext = _localctx;
      setState(43);
      match(ExprParser::T__9);
      setState(44);
      match(ExprParser::T__2);
      setState(45);
      portFieldExpr();
      setState(50);
      _errHandler->sync(this);

      _la = _input->LA(1);
      if (_la == ExprParser::T__10) {
        setState(46);
        match(ExprParser::T__10);
        setState(47);
        match(ExprParser::T__11);
        setState(48);
        match(ExprParser::COMPARISON);
        setState(49);
        expr(0);
      }
      setState(52);
      match(ExprParser::T__3);
      break;
    }

    case 7: {
      _localctx = _tracker.createInstance<TimeSumContext>(_localctx);
      _ctx = _localctx;
      previousContext = _localctx;
      setState(54);
      match(ExprParser::T__8);
      setState(55);
      match(ExprParser::T__2);
      setState(56);
      antlrcpp::downCast<TimeSumContext *>(_localctx)->from = sum_bound();
      setState(57);
      match(ExprParser::T__12);
      setState(58);
      antlrcpp::downCast<TimeSumContext *>(_localctx)->to = sum_bound();
      setState(59);
      match(ExprParser::T__10);
      setState(60);
      expr(0);
      setState(61);
      match(ExprParser::T__3);
      break;
    }

    case 8: {
      _localctx = _tracker.createInstance<FunctionContext>(_localctx);
      _ctx = _localctx;
      previousContext = _localctx;
      setState(63);
      match(ExprParser::IDENTIFIER);
      setState(64);
      match(ExprParser::T__2);
      setState(66);
      _errHandler->sync(this);

      _la = _input->LA(1);
      if ((((_la & ~ 0x3fULL) == 0) &&
        ((1ULL << _la) & 329228) != 0)) {
        setState(65);
        argList();
      }
      setState(68);
      match(ExprParser::T__3);
      break;
    }

    case 9: {
      _localctx = _tracker.createInstance<TimeShiftContext>(_localctx);
      _ctx = _localctx;
      previousContext = _localctx;
      setState(69);
      match(ExprParser::IDENTIFIER);
      setState(70);
      match(ExprParser::T__13);
      setState(71);
      shift();
      setState(72);
      match(ExprParser::T__14);
      break;
    }

    case 10: {
      _localctx = _tracker.createInstance<TimeIndexContext>(_localctx);
      _ctx = _localctx;
      previousContext = _localctx;
      setState(74);
      match(ExprParser::IDENTIFIER);
      setState(75);
      match(ExprParser::T__13);
      setState(76);
      expr(0);
      setState(77);
      match(ExprParser::T__14);
      break;
    }

    case 11: {
      _localctx = _tracker.createInstance<TimeShiftExprContext>(_localctx);
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
      shift();
      setState(84);
      match(ExprParser::T__14);
      break;
    }

    case 12: {
      _localctx = _tracker.createInstance<TimeIndexExprContext>(_localctx);
      _ctx = _localctx;
      previousContext = _localctx;
      setState(86);
      match(ExprParser::T__2);
      setState(87);
      expr(0);
      setState(88);
      match(ExprParser::T__3);
      setState(89);
      match(ExprParser::T__13);
      setState(90);
      expr(0);
      setState(91);
      match(ExprParser::T__14);
      break;
    }

    default:
      break;
    }
    _ctx->stop = _input->LT(-1);
    setState(109);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 5, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        if (!_parseListeners.empty())
          triggerExitRuleEvent();
        previousContext = _localctx;
        setState(107);
        _errHandler->sync(this);
        switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 4, _ctx)) {
        case 1: {
          auto newContext = _tracker.createInstance<PowerContext>(_tracker.createInstance<ExprContext>(parentContext, parentState));
          _localctx = newContext;
          pushNewRecursionContext(newContext, startState, RuleExpr);
          setState(95);

          if (!(precpred(_ctx, 12))) throw FailedPredicateException(this, "precpred(_ctx, 12)");
          setState(96);
          match(ExprParser::T__4);
          setState(97);
          expr(12);
          break;
        }

        case 2: {
          auto newContext = _tracker.createInstance<MuldivContext>(_tracker.createInstance<ExprContext>(parentContext, parentState));
          _localctx = newContext;
          pushNewRecursionContext(newContext, startState, RuleExpr);
          setState(98);

          if (!(precpred(_ctx, 11))) throw FailedPredicateException(this, "precpred(_ctx, 11)");
          setState(99);
          antlrcpp::downCast<MuldivContext *>(_localctx)->op = _input->LT(1);
          _la = _input->LA(1);
          if (!(_la == ExprParser::T__5

          || _la == ExprParser::T__6)) {
            antlrcpp::downCast<MuldivContext *>(_localctx)->op = _errHandler->recoverInline(this);
          }
          else {
            _errHandler->reportMatch(this);
            consume();
          }
          setState(100);
          expr(12);
          break;
        }

        case 3: {
          auto newContext = _tracker.createInstance<AddsubContext>(_tracker.createInstance<ExprContext>(parentContext, parentState));
          _localctx = newContext;
          pushNewRecursionContext(newContext, startState, RuleExpr);
          setState(101);

          if (!(precpred(_ctx, 10))) throw FailedPredicateException(this, "precpred(_ctx, 10)");
          setState(102);
          antlrcpp::downCast<AddsubContext *>(_localctx)->op = _input->LT(1);
          _la = _input->LA(1);
          if (!(_la == ExprParser::T__1

          || _la == ExprParser::T__7)) {
            antlrcpp::downCast<AddsubContext *>(_localctx)->op = _errHandler->recoverInline(this);
          }
          else {
            _errHandler->reportMatch(this);
            consume();
          }
          setState(103);
          expr(11);
          break;
        }

        case 4: {
          auto newContext = _tracker.createInstance<ComparisonContext>(_tracker.createInstance<ExprContext>(parentContext, parentState));
          _localctx = newContext;
          pushNewRecursionContext(newContext, startState, RuleExpr);
          setState(104);

          if (!(precpred(_ctx, 9))) throw FailedPredicateException(this, "precpred(_ctx, 9)");
          setState(105);
          match(ExprParser::COMPARISON);
          setState(106);
          expr(10);
          break;
        }

        default:
          break;
        } 
      }
      setState(111);
      _errHandler->sync(this);
      alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 5, _ctx);
    }
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }
  return _localctx;
}

//----------------- ArgListContext ------------------------------------------------------------------

ExprParser::ArgListContext::ArgListContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<ExprParser::ExprContext *> ExprParser::ArgListContext::expr() {
  return getRuleContexts<ExprParser::ExprContext>();
}

ExprParser::ExprContext* ExprParser::ArgListContext::expr(size_t i) {
  return getRuleContext<ExprParser::ExprContext>(i);
}


size_t ExprParser::ArgListContext::getRuleIndex() const {
  return ExprParser::RuleArgList;
}


std::any ExprParser::ArgListContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<ExprVisitor*>(visitor))
    return parserVisitor->visitArgList(this);
  else
    return visitor->visitChildren(this);
}

ExprParser::ArgListContext* ExprParser::argList() {
  ArgListContext *_localctx = _tracker.createInstance<ArgListContext>(_ctx, getState());
  enterRule(_localctx, 8, ExprParser::RuleArgList);
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
    setState(112);
    expr(0);
    setState(117);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == ExprParser::T__10) {
      setState(113);
      match(ExprParser::T__10);
      setState(114);
      expr(0);
      setState(119);
      _errHandler->sync(this);
      _la = _input->LA(1);
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
  enterRule(_localctx, 10, ExprParser::RuleAtom);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(122);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case ExprParser::NUMBER: {
        _localctx = _tracker.createInstance<ExprParser::NumberContext>(_localctx);
        enterOuterAlt(_localctx, 1);
        setState(120);
        match(ExprParser::NUMBER);
        break;
      }

      case ExprParser::IDENTIFIER: {
        _localctx = _tracker.createInstance<ExprParser::IdentifierContext>(_localctx);
        enterOuterAlt(_localctx, 2);
        setState(121);
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
  enterRule(_localctx, 12, ExprParser::RuleShift);
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
    setState(124);
    match(ExprParser::TIME);
    setState(126);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == ExprParser::T__1

    || _la == ExprParser::T__7) {
      setState(125);
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
//----------------- ShiftPowerContext ------------------------------------------------------------------

ExprParser::Shift_exprContext* ExprParser::ShiftPowerContext::shift_expr() {
  return getRuleContext<ExprParser::Shift_exprContext>(0);
}

ExprParser::Right_exprContext* ExprParser::ShiftPowerContext::right_expr() {
  return getRuleContext<ExprParser::Right_exprContext>(0);
}

ExprParser::ShiftPowerContext::ShiftPowerContext(Shift_exprContext *ctx) { copyFrom(ctx); }


std::any ExprParser::ShiftPowerContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<ExprVisitor*>(visitor))
    return parserVisitor->visitShiftPower(this);
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
  size_t startState = 14;
  enterRecursionRule(_localctx, 14, ExprParser::RuleShift_expr, precedence);

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
    setState(136);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 9, _ctx)) {
    case 1: {
      _localctx = _tracker.createInstance<SignedAtomContext>(_localctx);
      _ctx = _localctx;
      previousContext = _localctx;

      setState(129);
      antlrcpp::downCast<SignedAtomContext *>(_localctx)->op = _input->LT(1);
      _la = _input->LA(1);
      if (!(_la == ExprParser::T__1

      || _la == ExprParser::T__7)) {
        antlrcpp::downCast<SignedAtomContext *>(_localctx)->op = _errHandler->recoverInline(this);
      }
      else {
        _errHandler->reportMatch(this);
        consume();
      }
      setState(130);
      atom();
      break;
    }

    case 2: {
      _localctx = _tracker.createInstance<SignedExpressionContext>(_localctx);
      _ctx = _localctx;
      previousContext = _localctx;
      setState(131);
      antlrcpp::downCast<SignedExpressionContext *>(_localctx)->op = _input->LT(1);
      _la = _input->LA(1);
      if (!(_la == ExprParser::T__1

      || _la == ExprParser::T__7)) {
        antlrcpp::downCast<SignedExpressionContext *>(_localctx)->op = _errHandler->recoverInline(this);
      }
      else {
        _errHandler->reportMatch(this);
        consume();
      }
      setState(132);
      match(ExprParser::T__2);
      setState(133);
      expr(0);
      setState(134);
      match(ExprParser::T__3);
      break;
    }

    default:
      break;
    }
    _ctx->stop = _input->LT(-1);
    setState(149);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 11, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        if (!_parseListeners.empty())
          triggerExitRuleEvent();
        previousContext = _localctx;
        setState(147);
        _errHandler->sync(this);
        switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 10, _ctx)) {
        case 1: {
          auto newContext = _tracker.createInstance<ShiftPowerContext>(_tracker.createInstance<Shift_exprContext>(parentContext, parentState));
          _localctx = newContext;
          pushNewRecursionContext(newContext, startState, RuleShift_expr);
          setState(138);

          if (!(precpred(_ctx, 5))) throw FailedPredicateException(this, "precpred(_ctx, 5)");
          setState(139);
          match(ExprParser::T__4);
          setState(140);
          right_expr(0);
          break;
        }

        case 2: {
          auto newContext = _tracker.createInstance<ShiftMuldivContext>(_tracker.createInstance<Shift_exprContext>(parentContext, parentState));
          _localctx = newContext;
          pushNewRecursionContext(newContext, startState, RuleShift_expr);
          setState(141);

          if (!(precpred(_ctx, 4))) throw FailedPredicateException(this, "precpred(_ctx, 4)");
          setState(142);
          antlrcpp::downCast<ShiftMuldivContext *>(_localctx)->op = _input->LT(1);
          _la = _input->LA(1);
          if (!(_la == ExprParser::T__5

          || _la == ExprParser::T__6)) {
            antlrcpp::downCast<ShiftMuldivContext *>(_localctx)->op = _errHandler->recoverInline(this);
          }
          else {
            _errHandler->reportMatch(this);
            consume();
          }
          setState(143);
          right_expr(0);
          break;
        }

        case 3: {
          auto newContext = _tracker.createInstance<ShiftAddsubContext>(_tracker.createInstance<Shift_exprContext>(parentContext, parentState));
          _localctx = newContext;
          pushNewRecursionContext(newContext, startState, RuleShift_expr);
          setState(144);

          if (!(precpred(_ctx, 3))) throw FailedPredicateException(this, "precpred(_ctx, 3)");
          setState(145);
          antlrcpp::downCast<ShiftAddsubContext *>(_localctx)->op = _input->LT(1);
          _la = _input->LA(1);
          if (!(_la == ExprParser::T__1

          || _la == ExprParser::T__7)) {
            antlrcpp::downCast<ShiftAddsubContext *>(_localctx)->op = _errHandler->recoverInline(this);
          }
          else {
            _errHandler->reportMatch(this);
            consume();
          }
          setState(146);
          right_expr(0);
          break;
        }

        default:
          break;
        } 
      }
      setState(151);
      _errHandler->sync(this);
      alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 11, _ctx);
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
//----------------- RightPowerContext ------------------------------------------------------------------

std::vector<ExprParser::Right_exprContext *> ExprParser::RightPowerContext::right_expr() {
  return getRuleContexts<ExprParser::Right_exprContext>();
}

ExprParser::Right_exprContext* ExprParser::RightPowerContext::right_expr(size_t i) {
  return getRuleContext<ExprParser::Right_exprContext>(i);
}

ExprParser::RightPowerContext::RightPowerContext(Right_exprContext *ctx) { copyFrom(ctx); }


std::any ExprParser::RightPowerContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<ExprVisitor*>(visitor))
    return parserVisitor->visitRightPower(this);
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
  size_t startState = 16;
  enterRecursionRule(_localctx, 16, ExprParser::RuleRight_expr, precedence);

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
    setState(158);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case ExprParser::T__2: {
        _localctx = _tracker.createInstance<RightExpressionContext>(_localctx);
        _ctx = _localctx;
        previousContext = _localctx;

        setState(153);
        match(ExprParser::T__2);
        setState(154);
        expr(0);
        setState(155);
        match(ExprParser::T__3);
        break;
      }

      case ExprParser::NUMBER:
      case ExprParser::IDENTIFIER: {
        _localctx = _tracker.createInstance<RightAtomContext>(_localctx);
        _ctx = _localctx;
        previousContext = _localctx;
        setState(157);
        atom();
        break;
      }

    default:
      throw NoViableAltException(this);
    }
    _ctx->stop = _input->LT(-1);
    setState(168);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 14, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        if (!_parseListeners.empty())
          triggerExitRuleEvent();
        previousContext = _localctx;
        setState(166);
        _errHandler->sync(this);
        switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 13, _ctx)) {
        case 1: {
          auto newContext = _tracker.createInstance<RightPowerContext>(_tracker.createInstance<Right_exprContext>(parentContext, parentState));
          _localctx = newContext;
          pushNewRecursionContext(newContext, startState, RuleRight_expr);
          setState(160);

          if (!(precpred(_ctx, 4))) throw FailedPredicateException(this, "precpred(_ctx, 4)");
          setState(161);
          match(ExprParser::T__4);
          setState(162);
          right_expr(4);
          break;
        }

        case 2: {
          auto newContext = _tracker.createInstance<RightMuldivContext>(_tracker.createInstance<Right_exprContext>(parentContext, parentState));
          _localctx = newContext;
          pushNewRecursionContext(newContext, startState, RuleRight_expr);
          setState(163);

          if (!(precpred(_ctx, 3))) throw FailedPredicateException(this, "precpred(_ctx, 3)");
          setState(164);
          antlrcpp::downCast<RightMuldivContext *>(_localctx)->op = _input->LT(1);
          _la = _input->LA(1);
          if (!(_la == ExprParser::T__5

          || _la == ExprParser::T__6)) {
            antlrcpp::downCast<RightMuldivContext *>(_localctx)->op = _errHandler->recoverInline(this);
          }
          else {
            _errHandler->reportMatch(this);
            consume();
          }
          setState(165);
          right_expr(4);
          break;
        }

        default:
          break;
        } 
      }
      setState(170);
      _errHandler->sync(this);
      alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 14, _ctx);
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
    case 3: return exprSempred(antlrcpp::downCast<ExprContext *>(context), predicateIndex);
    case 7: return shift_exprSempred(antlrcpp::downCast<Shift_exprContext *>(context), predicateIndex);
    case 8: return right_exprSempred(antlrcpp::downCast<Right_exprContext *>(context), predicateIndex);

  default:
    break;
  }
  return true;
}

bool ExprParser::exprSempred(ExprContext *_localctx, size_t predicateIndex) {
  switch (predicateIndex) {
    case 0: return precpred(_ctx, 12);
    case 1: return precpred(_ctx, 11);
    case 2: return precpred(_ctx, 10);
    case 3: return precpred(_ctx, 9);

  default:
    break;
  }
  return true;
}

bool ExprParser::shift_exprSempred(Shift_exprContext *_localctx, size_t predicateIndex) {
  switch (predicateIndex) {
    case 4: return precpred(_ctx, 5);
    case 5: return precpred(_ctx, 4);
    case 6: return precpred(_ctx, 3);

  default:
    break;
  }
  return true;
}

bool ExprParser::right_exprSempred(Right_exprContext *_localctx, size_t predicateIndex) {
  switch (predicateIndex) {
    case 7: return precpred(_ctx, 4);
    case 8: return precpred(_ctx, 3);

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
