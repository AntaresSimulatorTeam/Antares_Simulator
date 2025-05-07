
// Generated from HoursField.g4 by ANTLR 4.13.2


#include "HoursFieldVisitor.h"

#include "HoursFieldParser.h"


using namespace antlrcpp;

using namespace antlr4;

namespace {

struct HoursFieldParserStaticData final {
  HoursFieldParserStaticData(std::vector<std::string> ruleNames,
                        std::vector<std::string> literalNames,
                        std::vector<std::string> symbolicNames)
      : ruleNames(std::move(ruleNames)), literalNames(std::move(literalNames)),
        symbolicNames(std::move(symbolicNames)),
        vocabulary(this->literalNames, this->symbolicNames) {}

  HoursFieldParserStaticData(const HoursFieldParserStaticData&) = delete;
  HoursFieldParserStaticData(HoursFieldParserStaticData&&) = delete;
  HoursFieldParserStaticData& operator=(const HoursFieldParserStaticData&) = delete;
  HoursFieldParserStaticData& operator=(HoursFieldParserStaticData&&) = delete;

  std::vector<antlr4::dfa::DFA> decisionToDFA;
  antlr4::atn::PredictionContextCache sharedContextCache;
  const std::vector<std::string> ruleNames;
  const std::vector<std::string> literalNames;
  const std::vector<std::string> symbolicNames;
  const antlr4::dfa::Vocabulary vocabulary;
  antlr4::atn::SerializedATNView serializedATN;
  std::unique_ptr<antlr4::atn::ATN> atn;
};

::antlr4::internal::OnceFlag hoursfieldParserOnceFlag;
#if ANTLR4_USE_THREAD_LOCAL_CACHE
static thread_local
#endif
std::unique_ptr<HoursFieldParserStaticData> hoursfieldParserStaticData = nullptr;

void hoursfieldParserInitialize() {
#if ANTLR4_USE_THREAD_LOCAL_CACHE
  if (hoursfieldParserStaticData != nullptr) {
    return;
  }
#else
  assert(hoursfieldParserStaticData == nullptr);
#endif
  auto staticData = std::make_unique<HoursFieldParserStaticData>(
    std::vector<std::string>{
      "hoursField", "group", "hour"
    },
    std::vector<std::string>{
      "", "'['", "']'", "','"
    },
    std::vector<std::string>{
      "", "LBRACK", "RBRACK", "COMMA", "INT", "WS"
    }
  );
  static const int32_t serializedATNSegment[] = {
  	4,1,5,48,2,0,7,0,2,1,7,1,2,2,7,2,1,0,1,0,1,0,5,0,10,8,0,10,0,12,0,13,
  	9,0,1,0,1,0,1,1,1,1,5,1,19,8,1,10,1,12,1,22,9,1,1,1,1,1,1,1,5,1,27,8,
  	1,10,1,12,1,30,9,1,1,1,5,1,33,8,1,10,1,12,1,36,9,1,1,1,5,1,39,8,1,10,
  	1,12,1,42,9,1,1,1,1,1,1,2,1,2,1,2,0,0,3,0,2,4,0,0,49,0,6,1,0,0,0,2,16,
  	1,0,0,0,4,45,1,0,0,0,6,11,3,2,1,0,7,8,5,3,0,0,8,10,3,2,1,0,9,7,1,0,0,
  	0,10,13,1,0,0,0,11,9,1,0,0,0,11,12,1,0,0,0,12,14,1,0,0,0,13,11,1,0,0,
  	0,14,15,5,0,0,1,15,1,1,0,0,0,16,20,5,1,0,0,17,19,5,5,0,0,18,17,1,0,0,
  	0,19,22,1,0,0,0,20,18,1,0,0,0,20,21,1,0,0,0,21,23,1,0,0,0,22,20,1,0,0,
  	0,23,34,3,4,2,0,24,28,5,3,0,0,25,27,5,5,0,0,26,25,1,0,0,0,27,30,1,0,0,
  	0,28,26,1,0,0,0,28,29,1,0,0,0,29,31,1,0,0,0,30,28,1,0,0,0,31,33,3,4,2,
  	0,32,24,1,0,0,0,33,36,1,0,0,0,34,32,1,0,0,0,34,35,1,0,0,0,35,40,1,0,0,
  	0,36,34,1,0,0,0,37,39,5,5,0,0,38,37,1,0,0,0,39,42,1,0,0,0,40,38,1,0,0,
  	0,40,41,1,0,0,0,41,43,1,0,0,0,42,40,1,0,0,0,43,44,5,2,0,0,44,3,1,0,0,
  	0,45,46,5,4,0,0,46,5,1,0,0,0,5,11,20,28,34,40
  };
  staticData->serializedATN = antlr4::atn::SerializedATNView(serializedATNSegment, sizeof(serializedATNSegment) / sizeof(serializedATNSegment[0]));

  antlr4::atn::ATNDeserializer deserializer;
  staticData->atn = deserializer.deserialize(staticData->serializedATN);

  const size_t count = staticData->atn->getNumberOfDecisions();
  staticData->decisionToDFA.reserve(count);
  for (size_t i = 0; i < count; i++) { 
    staticData->decisionToDFA.emplace_back(staticData->atn->getDecisionState(i), i);
  }
  hoursfieldParserStaticData = std::move(staticData);
}

}

HoursFieldParser::HoursFieldParser(TokenStream *input) : HoursFieldParser(input, antlr4::atn::ParserATNSimulatorOptions()) {}

HoursFieldParser::HoursFieldParser(TokenStream *input, const antlr4::atn::ParserATNSimulatorOptions &options) : Parser(input) {
  HoursFieldParser::initialize();
  _interpreter = new atn::ParserATNSimulator(this, *hoursfieldParserStaticData->atn, hoursfieldParserStaticData->decisionToDFA, hoursfieldParserStaticData->sharedContextCache, options);
}

HoursFieldParser::~HoursFieldParser() {
  delete _interpreter;
}

const atn::ATN& HoursFieldParser::getATN() const {
  return *hoursfieldParserStaticData->atn;
}

std::string HoursFieldParser::getGrammarFileName() const {
  return "HoursField.g4";
}

const std::vector<std::string>& HoursFieldParser::getRuleNames() const {
  return hoursfieldParserStaticData->ruleNames;
}

const dfa::Vocabulary& HoursFieldParser::getVocabulary() const {
  return hoursfieldParserStaticData->vocabulary;
}

antlr4::atn::SerializedATNView HoursFieldParser::getSerializedATN() const {
  return hoursfieldParserStaticData->serializedATN;
}


//----------------- HoursFieldContext ------------------------------------------------------------------

HoursFieldParser::HoursFieldContext::HoursFieldContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<HoursFieldParser::GroupContext *> HoursFieldParser::HoursFieldContext::group() {
  return getRuleContexts<HoursFieldParser::GroupContext>();
}

HoursFieldParser::GroupContext* HoursFieldParser::HoursFieldContext::group(size_t i) {
  return getRuleContext<HoursFieldParser::GroupContext>(i);
}

tree::TerminalNode* HoursFieldParser::HoursFieldContext::EOF() {
  return getToken(HoursFieldParser::EOF, 0);
}

std::vector<tree::TerminalNode *> HoursFieldParser::HoursFieldContext::COMMA() {
  return getTokens(HoursFieldParser::COMMA);
}

tree::TerminalNode* HoursFieldParser::HoursFieldContext::COMMA(size_t i) {
  return getToken(HoursFieldParser::COMMA, i);
}


size_t HoursFieldParser::HoursFieldContext::getRuleIndex() const {
  return HoursFieldParser::RuleHoursField;
}


std::any HoursFieldParser::HoursFieldContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<HoursFieldVisitor*>(visitor))
    return parserVisitor->visitHoursField(this);
  else
    return visitor->visitChildren(this);
}

HoursFieldParser::HoursFieldContext* HoursFieldParser::hoursField() {
  HoursFieldContext *_localctx = _tracker.createInstance<HoursFieldContext>(_ctx, getState());
  enterRule(_localctx, 0, HoursFieldParser::RuleHoursField);
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
    setState(6);
    group();
    setState(11);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == HoursFieldParser::COMMA) {
      setState(7);
      match(HoursFieldParser::COMMA);
      setState(8);
      group();
      setState(13);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(14);
    match(HoursFieldParser::EOF);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- GroupContext ------------------------------------------------------------------

HoursFieldParser::GroupContext::GroupContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* HoursFieldParser::GroupContext::LBRACK() {
  return getToken(HoursFieldParser::LBRACK, 0);
}

std::vector<HoursFieldParser::HourContext *> HoursFieldParser::GroupContext::hour() {
  return getRuleContexts<HoursFieldParser::HourContext>();
}

HoursFieldParser::HourContext* HoursFieldParser::GroupContext::hour(size_t i) {
  return getRuleContext<HoursFieldParser::HourContext>(i);
}

tree::TerminalNode* HoursFieldParser::GroupContext::RBRACK() {
  return getToken(HoursFieldParser::RBRACK, 0);
}

std::vector<tree::TerminalNode *> HoursFieldParser::GroupContext::WS() {
  return getTokens(HoursFieldParser::WS);
}

tree::TerminalNode* HoursFieldParser::GroupContext::WS(size_t i) {
  return getToken(HoursFieldParser::WS, i);
}

std::vector<tree::TerminalNode *> HoursFieldParser::GroupContext::COMMA() {
  return getTokens(HoursFieldParser::COMMA);
}

tree::TerminalNode* HoursFieldParser::GroupContext::COMMA(size_t i) {
  return getToken(HoursFieldParser::COMMA, i);
}


size_t HoursFieldParser::GroupContext::getRuleIndex() const {
  return HoursFieldParser::RuleGroup;
}


std::any HoursFieldParser::GroupContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<HoursFieldVisitor*>(visitor))
    return parserVisitor->visitGroup(this);
  else
    return visitor->visitChildren(this);
}

HoursFieldParser::GroupContext* HoursFieldParser::group() {
  GroupContext *_localctx = _tracker.createInstance<GroupContext>(_ctx, getState());
  enterRule(_localctx, 2, HoursFieldParser::RuleGroup);
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
    setState(16);
    match(HoursFieldParser::LBRACK);
    setState(20);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == HoursFieldParser::WS) {
      setState(17);
      match(HoursFieldParser::WS);
      setState(22);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(23);
    hour();
    setState(34);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == HoursFieldParser::COMMA) {
      setState(24);
      match(HoursFieldParser::COMMA);
      setState(28);
      _errHandler->sync(this);
      _la = _input->LA(1);
      while (_la == HoursFieldParser::WS) {
        setState(25);
        match(HoursFieldParser::WS);
        setState(30);
        _errHandler->sync(this);
        _la = _input->LA(1);
      }
      setState(31);
      hour();
      setState(36);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(40);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == HoursFieldParser::WS) {
      setState(37);
      match(HoursFieldParser::WS);
      setState(42);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(43);
    match(HoursFieldParser::RBRACK);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- HourContext ------------------------------------------------------------------

HoursFieldParser::HourContext::HourContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* HoursFieldParser::HourContext::INT() {
  return getToken(HoursFieldParser::INT, 0);
}


size_t HoursFieldParser::HourContext::getRuleIndex() const {
  return HoursFieldParser::RuleHour;
}


std::any HoursFieldParser::HourContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<HoursFieldVisitor*>(visitor))
    return parserVisitor->visitHour(this);
  else
    return visitor->visitChildren(this);
}

HoursFieldParser::HourContext* HoursFieldParser::hour() {
  HourContext *_localctx = _tracker.createInstance<HourContext>(_ctx, getState());
  enterRule(_localctx, 4, HoursFieldParser::RuleHour);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(45);
    match(HoursFieldParser::INT);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

void HoursFieldParser::initialize() {
#if ANTLR4_USE_THREAD_LOCAL_CACHE
  hoursfieldParserInitialize();
#else
  ::antlr4::internal::call_once(hoursfieldParserOnceFlag, hoursfieldParserInitialize);
#endif
}
