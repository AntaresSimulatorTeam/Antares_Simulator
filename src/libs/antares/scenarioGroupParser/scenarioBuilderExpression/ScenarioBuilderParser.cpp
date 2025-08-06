
// Generated from ScenarioBuilder.g4 by ANTLR 4.13.2


#include "ScenarioBuilderVisitor.h"

#include "ScenarioBuilderParser.h"


using namespace antlrcpp;

using namespace antlr4;

namespace {

struct ScenarioBuilderParserStaticData final {
  ScenarioBuilderParserStaticData(std::vector<std::string> ruleNames,
                        std::vector<std::string> literalNames,
                        std::vector<std::string> symbolicNames)
      : ruleNames(std::move(ruleNames)), literalNames(std::move(literalNames)),
        symbolicNames(std::move(symbolicNames)),
        vocabulary(this->literalNames, this->symbolicNames) {}

  ScenarioBuilderParserStaticData(const ScenarioBuilderParserStaticData&) = delete;
  ScenarioBuilderParserStaticData(ScenarioBuilderParserStaticData&&) = delete;
  ScenarioBuilderParserStaticData& operator=(const ScenarioBuilderParserStaticData&) = delete;
  ScenarioBuilderParserStaticData& operator=(ScenarioBuilderParserStaticData&&) = delete;

  std::vector<antlr4::dfa::DFA> decisionToDFA;
  antlr4::atn::PredictionContextCache sharedContextCache;
  const std::vector<std::string> ruleNames;
  const std::vector<std::string> literalNames;
  const std::vector<std::string> symbolicNames;
  const antlr4::dfa::Vocabulary vocabulary;
  antlr4::atn::SerializedATNView serializedATN;
  std::unique_ptr<antlr4::atn::ATN> atn;
};

::antlr4::internal::OnceFlag scenariobuilderParserOnceFlag;
#if ANTLR4_USE_THREAD_LOCAL_CACHE
static thread_local
#endif
std::unique_ptr<ScenarioBuilderParserStaticData> scenariobuilderParserStaticData = nullptr;

void scenariobuilderParserInitialize() {
#if ANTLR4_USE_THREAD_LOCAL_CACHE
  if (scenariobuilderParserStaticData != nullptr) {
    return;
  }
#else
  assert(scenariobuilderParserStaticData == nullptr);
#endif
  auto staticData = std::make_unique<ScenarioBuilderParserStaticData>(
    std::vector<std::string>{
      "rules", "line", "group", "year", "timeSeriesNumber"
    },
    std::vector<std::string>{
      "", "','", "", "", "", "'='"
    },
    std::vector<std::string>{
      "", "COMMA", "INT", "WS", "IDENTIFIER", "EQUALS"
    }
  );
  static const int32_t serializedATNSegment[] = {
  	4,1,5,30,2,0,7,0,2,1,7,1,2,2,7,2,2,3,7,3,2,4,7,4,1,0,4,0,12,8,0,11,0,
  	12,0,13,1,0,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,2,1,2,1,3,1,3,1,4,1,4,1,4,0,
  	0,5,0,2,4,6,8,0,0,25,0,11,1,0,0,0,2,17,1,0,0,0,4,23,1,0,0,0,6,25,1,0,
  	0,0,8,27,1,0,0,0,10,12,3,2,1,0,11,10,1,0,0,0,12,13,1,0,0,0,13,11,1,0,
  	0,0,13,14,1,0,0,0,14,15,1,0,0,0,15,16,5,0,0,1,16,1,1,0,0,0,17,18,3,4,
  	2,0,18,19,5,1,0,0,19,20,3,6,3,0,20,21,5,5,0,0,21,22,3,8,4,0,22,3,1,0,
  	0,0,23,24,5,4,0,0,24,5,1,0,0,0,25,26,5,2,0,0,26,7,1,0,0,0,27,28,5,2,0,
  	0,28,9,1,0,0,0,1,13
  };
  staticData->serializedATN = antlr4::atn::SerializedATNView(serializedATNSegment, sizeof(serializedATNSegment) / sizeof(serializedATNSegment[0]));

  antlr4::atn::ATNDeserializer deserializer;
  staticData->atn = deserializer.deserialize(staticData->serializedATN);

  const size_t count = staticData->atn->getNumberOfDecisions();
  staticData->decisionToDFA.reserve(count);
  for (size_t i = 0; i < count; i++) { 
    staticData->decisionToDFA.emplace_back(staticData->atn->getDecisionState(i), i);
  }
  scenariobuilderParserStaticData = std::move(staticData);
}

}

ScenarioBuilderParser::ScenarioBuilderParser(TokenStream *input) : ScenarioBuilderParser(input, antlr4::atn::ParserATNSimulatorOptions()) {}

ScenarioBuilderParser::ScenarioBuilderParser(TokenStream *input, const antlr4::atn::ParserATNSimulatorOptions &options) : Parser(input) {
  ScenarioBuilderParser::initialize();
  _interpreter = new atn::ParserATNSimulator(this, *scenariobuilderParserStaticData->atn, scenariobuilderParserStaticData->decisionToDFA, scenariobuilderParserStaticData->sharedContextCache, options);
}

ScenarioBuilderParser::~ScenarioBuilderParser() {
  delete _interpreter;
}

const atn::ATN& ScenarioBuilderParser::getATN() const {
  return *scenariobuilderParserStaticData->atn;
}

std::string ScenarioBuilderParser::getGrammarFileName() const {
  return "ScenarioBuilder.g4";
}

const std::vector<std::string>& ScenarioBuilderParser::getRuleNames() const {
  return scenariobuilderParserStaticData->ruleNames;
}

const dfa::Vocabulary& ScenarioBuilderParser::getVocabulary() const {
  return scenariobuilderParserStaticData->vocabulary;
}

antlr4::atn::SerializedATNView ScenarioBuilderParser::getSerializedATN() const {
  return scenariobuilderParserStaticData->serializedATN;
}


//----------------- RulesContext ------------------------------------------------------------------

ScenarioBuilderParser::RulesContext::RulesContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* ScenarioBuilderParser::RulesContext::EOF() {
  return getToken(ScenarioBuilderParser::EOF, 0);
}

std::vector<ScenarioBuilderParser::LineContext *> ScenarioBuilderParser::RulesContext::line() {
  return getRuleContexts<ScenarioBuilderParser::LineContext>();
}

ScenarioBuilderParser::LineContext* ScenarioBuilderParser::RulesContext::line(size_t i) {
  return getRuleContext<ScenarioBuilderParser::LineContext>(i);
}


size_t ScenarioBuilderParser::RulesContext::getRuleIndex() const {
  return ScenarioBuilderParser::RuleRules;
}


std::any ScenarioBuilderParser::RulesContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<ScenarioBuilderVisitor*>(visitor))
    return parserVisitor->visitRules(this);
  else
    return visitor->visitChildren(this);
}

ScenarioBuilderParser::RulesContext* ScenarioBuilderParser::rules() {
  RulesContext *_localctx = _tracker.createInstance<RulesContext>(_ctx, getState());
  enterRule(_localctx, 0, ScenarioBuilderParser::RuleRules);
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
    setState(11); 
    _errHandler->sync(this);
    _la = _input->LA(1);
    do {
      setState(10);
      line();
      setState(13); 
      _errHandler->sync(this);
      _la = _input->LA(1);
    } while (_la == ScenarioBuilderParser::IDENTIFIER);
    setState(15);
    match(ScenarioBuilderParser::EOF);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- LineContext ------------------------------------------------------------------

ScenarioBuilderParser::LineContext::LineContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

ScenarioBuilderParser::GroupContext* ScenarioBuilderParser::LineContext::group() {
  return getRuleContext<ScenarioBuilderParser::GroupContext>(0);
}

tree::TerminalNode* ScenarioBuilderParser::LineContext::COMMA() {
  return getToken(ScenarioBuilderParser::COMMA, 0);
}

ScenarioBuilderParser::YearContext* ScenarioBuilderParser::LineContext::year() {
  return getRuleContext<ScenarioBuilderParser::YearContext>(0);
}

tree::TerminalNode* ScenarioBuilderParser::LineContext::EQUALS() {
  return getToken(ScenarioBuilderParser::EQUALS, 0);
}

ScenarioBuilderParser::TimeSeriesNumberContext* ScenarioBuilderParser::LineContext::timeSeriesNumber() {
  return getRuleContext<ScenarioBuilderParser::TimeSeriesNumberContext>(0);
}


size_t ScenarioBuilderParser::LineContext::getRuleIndex() const {
  return ScenarioBuilderParser::RuleLine;
}


std::any ScenarioBuilderParser::LineContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<ScenarioBuilderVisitor*>(visitor))
    return parserVisitor->visitLine(this);
  else
    return visitor->visitChildren(this);
}

ScenarioBuilderParser::LineContext* ScenarioBuilderParser::line() {
  LineContext *_localctx = _tracker.createInstance<LineContext>(_ctx, getState());
  enterRule(_localctx, 2, ScenarioBuilderParser::RuleLine);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(17);
    group();
    setState(18);
    match(ScenarioBuilderParser::COMMA);
    setState(19);
    year();
    setState(20);
    match(ScenarioBuilderParser::EQUALS);
    setState(21);
    timeSeriesNumber();
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- GroupContext ------------------------------------------------------------------

ScenarioBuilderParser::GroupContext::GroupContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* ScenarioBuilderParser::GroupContext::IDENTIFIER() {
  return getToken(ScenarioBuilderParser::IDENTIFIER, 0);
}


size_t ScenarioBuilderParser::GroupContext::getRuleIndex() const {
  return ScenarioBuilderParser::RuleGroup;
}


std::any ScenarioBuilderParser::GroupContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<ScenarioBuilderVisitor*>(visitor))
    return parserVisitor->visitGroup(this);
  else
    return visitor->visitChildren(this);
}

ScenarioBuilderParser::GroupContext* ScenarioBuilderParser::group() {
  GroupContext *_localctx = _tracker.createInstance<GroupContext>(_ctx, getState());
  enterRule(_localctx, 4, ScenarioBuilderParser::RuleGroup);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(23);
    match(ScenarioBuilderParser::IDENTIFIER);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- YearContext ------------------------------------------------------------------

ScenarioBuilderParser::YearContext::YearContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* ScenarioBuilderParser::YearContext::INT() {
  return getToken(ScenarioBuilderParser::INT, 0);
}


size_t ScenarioBuilderParser::YearContext::getRuleIndex() const {
  return ScenarioBuilderParser::RuleYear;
}


std::any ScenarioBuilderParser::YearContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<ScenarioBuilderVisitor*>(visitor))
    return parserVisitor->visitYear(this);
  else
    return visitor->visitChildren(this);
}

ScenarioBuilderParser::YearContext* ScenarioBuilderParser::year() {
  YearContext *_localctx = _tracker.createInstance<YearContext>(_ctx, getState());
  enterRule(_localctx, 6, ScenarioBuilderParser::RuleYear);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(25);
    match(ScenarioBuilderParser::INT);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- TimeSeriesNumberContext ------------------------------------------------------------------

ScenarioBuilderParser::TimeSeriesNumberContext::TimeSeriesNumberContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* ScenarioBuilderParser::TimeSeriesNumberContext::INT() {
  return getToken(ScenarioBuilderParser::INT, 0);
}


size_t ScenarioBuilderParser::TimeSeriesNumberContext::getRuleIndex() const {
  return ScenarioBuilderParser::RuleTimeSeriesNumber;
}


std::any ScenarioBuilderParser::TimeSeriesNumberContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<ScenarioBuilderVisitor*>(visitor))
    return parserVisitor->visitTimeSeriesNumber(this);
  else
    return visitor->visitChildren(this);
}

ScenarioBuilderParser::TimeSeriesNumberContext* ScenarioBuilderParser::timeSeriesNumber() {
  TimeSeriesNumberContext *_localctx = _tracker.createInstance<TimeSeriesNumberContext>(_ctx, getState());
  enterRule(_localctx, 8, ScenarioBuilderParser::RuleTimeSeriesNumber);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(27);
    match(ScenarioBuilderParser::INT);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

void ScenarioBuilderParser::initialize() {
#if ANTLR4_USE_THREAD_LOCAL_CACHE
  scenariobuilderParserInitialize();
#else
  ::antlr4::internal::call_once(scenariobuilderParserOnceFlag, scenariobuilderParserInitialize);
#endif
}
