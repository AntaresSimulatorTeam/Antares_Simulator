
// Generated from ScenarioBuilder.g4 by ANTLR 4.13.2


#include "ScenarioBuilderLexer.h"


using namespace antlr4;



using namespace antlr4;

namespace {

struct ScenarioBuilderLexerStaticData final {
  ScenarioBuilderLexerStaticData(std::vector<std::string> ruleNames,
                          std::vector<std::string> channelNames,
                          std::vector<std::string> modeNames,
                          std::vector<std::string> literalNames,
                          std::vector<std::string> symbolicNames)
      : ruleNames(std::move(ruleNames)), channelNames(std::move(channelNames)),
        modeNames(std::move(modeNames)), literalNames(std::move(literalNames)),
        symbolicNames(std::move(symbolicNames)),
        vocabulary(this->literalNames, this->symbolicNames) {}

  ScenarioBuilderLexerStaticData(const ScenarioBuilderLexerStaticData&) = delete;
  ScenarioBuilderLexerStaticData(ScenarioBuilderLexerStaticData&&) = delete;
  ScenarioBuilderLexerStaticData& operator=(const ScenarioBuilderLexerStaticData&) = delete;
  ScenarioBuilderLexerStaticData& operator=(ScenarioBuilderLexerStaticData&&) = delete;

  std::vector<antlr4::dfa::DFA> decisionToDFA;
  antlr4::atn::PredictionContextCache sharedContextCache;
  const std::vector<std::string> ruleNames;
  const std::vector<std::string> channelNames;
  const std::vector<std::string> modeNames;
  const std::vector<std::string> literalNames;
  const std::vector<std::string> symbolicNames;
  const antlr4::dfa::Vocabulary vocabulary;
  antlr4::atn::SerializedATNView serializedATN;
  std::unique_ptr<antlr4::atn::ATN> atn;
};

::antlr4::internal::OnceFlag scenariobuilderlexerLexerOnceFlag;
#if ANTLR4_USE_THREAD_LOCAL_CACHE
static thread_local
#endif
std::unique_ptr<ScenarioBuilderLexerStaticData> scenariobuilderlexerLexerStaticData = nullptr;

void scenariobuilderlexerLexerInitialize() {
#if ANTLR4_USE_THREAD_LOCAL_CACHE
  if (scenariobuilderlexerLexerStaticData != nullptr) {
    return;
  }
#else
  assert(scenariobuilderlexerLexerStaticData == nullptr);
#endif
  auto staticData = std::make_unique<ScenarioBuilderLexerStaticData>(
    std::vector<std::string>{
      "COMMA", "INT", "WS", "IDENTIFIER", "EQUALS"
    },
    std::vector<std::string>{
      "DEFAULT_TOKEN_CHANNEL", "HIDDEN"
    },
    std::vector<std::string>{
      "DEFAULT_MODE"
    },
    std::vector<std::string>{
      "", "','", "", "", "", "'='"
    },
    std::vector<std::string>{
      "", "COMMA", "INT", "WS", "IDENTIFIER", "EQUALS"
    }
  );
  static const int32_t serializedATNSegment[] = {
  	4,0,5,34,6,-1,2,0,7,0,2,1,7,1,2,2,7,2,2,3,7,3,2,4,7,4,1,0,1,0,1,1,4,1,
  	15,8,1,11,1,12,1,16,1,2,4,2,20,8,2,11,2,12,2,21,1,2,1,2,1,3,1,3,5,3,28,
  	8,3,10,3,12,3,31,9,3,1,4,1,4,0,0,5,1,1,3,2,5,3,7,4,9,5,1,0,4,1,0,48,57,
  	2,0,9,13,32,32,3,0,65,90,95,95,97,122,4,0,48,57,65,90,95,95,97,122,36,
  	0,1,1,0,0,0,0,3,1,0,0,0,0,5,1,0,0,0,0,7,1,0,0,0,0,9,1,0,0,0,1,11,1,0,
  	0,0,3,14,1,0,0,0,5,19,1,0,0,0,7,25,1,0,0,0,9,32,1,0,0,0,11,12,5,44,0,
  	0,12,2,1,0,0,0,13,15,7,0,0,0,14,13,1,0,0,0,15,16,1,0,0,0,16,14,1,0,0,
  	0,16,17,1,0,0,0,17,4,1,0,0,0,18,20,7,1,0,0,19,18,1,0,0,0,20,21,1,0,0,
  	0,21,19,1,0,0,0,21,22,1,0,0,0,22,23,1,0,0,0,23,24,6,2,0,0,24,6,1,0,0,
  	0,25,29,7,2,0,0,26,28,7,3,0,0,27,26,1,0,0,0,28,31,1,0,0,0,29,27,1,0,0,
  	0,29,30,1,0,0,0,30,8,1,0,0,0,31,29,1,0,0,0,32,33,5,61,0,0,33,10,1,0,0,
  	0,4,0,16,21,29,1,6,0,0
  };
  staticData->serializedATN = antlr4::atn::SerializedATNView(serializedATNSegment, sizeof(serializedATNSegment) / sizeof(serializedATNSegment[0]));

  antlr4::atn::ATNDeserializer deserializer;
  staticData->atn = deserializer.deserialize(staticData->serializedATN);

  const size_t count = staticData->atn->getNumberOfDecisions();
  staticData->decisionToDFA.reserve(count);
  for (size_t i = 0; i < count; i++) { 
    staticData->decisionToDFA.emplace_back(staticData->atn->getDecisionState(i), i);
  }
  scenariobuilderlexerLexerStaticData = std::move(staticData);
}

}

ScenarioBuilderLexer::ScenarioBuilderLexer(CharStream *input) : Lexer(input) {
  ScenarioBuilderLexer::initialize();
  _interpreter = new atn::LexerATNSimulator(this, *scenariobuilderlexerLexerStaticData->atn, scenariobuilderlexerLexerStaticData->decisionToDFA, scenariobuilderlexerLexerStaticData->sharedContextCache);
}

ScenarioBuilderLexer::~ScenarioBuilderLexer() {
  delete _interpreter;
}

std::string ScenarioBuilderLexer::getGrammarFileName() const {
  return "ScenarioBuilder.g4";
}

const std::vector<std::string>& ScenarioBuilderLexer::getRuleNames() const {
  return scenariobuilderlexerLexerStaticData->ruleNames;
}

const std::vector<std::string>& ScenarioBuilderLexer::getChannelNames() const {
  return scenariobuilderlexerLexerStaticData->channelNames;
}

const std::vector<std::string>& ScenarioBuilderLexer::getModeNames() const {
  return scenariobuilderlexerLexerStaticData->modeNames;
}

const dfa::Vocabulary& ScenarioBuilderLexer::getVocabulary() const {
  return scenariobuilderlexerLexerStaticData->vocabulary;
}

antlr4::atn::SerializedATNView ScenarioBuilderLexer::getSerializedATN() const {
  return scenariobuilderlexerLexerStaticData->serializedATN;
}

const atn::ATN& ScenarioBuilderLexer::getATN() const {
  return *scenariobuilderlexerLexerStaticData->atn;
}




void ScenarioBuilderLexer::initialize() {
#if ANTLR4_USE_THREAD_LOCAL_CACHE
  scenariobuilderlexerLexerInitialize();
#else
  ::antlr4::internal::call_once(scenariobuilderlexerLexerOnceFlag, scenariobuilderlexerLexerInitialize);
#endif
}
