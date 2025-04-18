
// Generated from HoursField.g4 by ANTLR 4.13.2


#include "HoursFieldLexer.h"


using namespace antlr4;



using namespace antlr4;

namespace {

struct HoursFieldLexerStaticData final {
  HoursFieldLexerStaticData(std::vector<std::string> ruleNames,
                          std::vector<std::string> channelNames,
                          std::vector<std::string> modeNames,
                          std::vector<std::string> literalNames,
                          std::vector<std::string> symbolicNames)
      : ruleNames(std::move(ruleNames)), channelNames(std::move(channelNames)),
        modeNames(std::move(modeNames)), literalNames(std::move(literalNames)),
        symbolicNames(std::move(symbolicNames)),
        vocabulary(this->literalNames, this->symbolicNames) {}

  HoursFieldLexerStaticData(const HoursFieldLexerStaticData&) = delete;
  HoursFieldLexerStaticData(HoursFieldLexerStaticData&&) = delete;
  HoursFieldLexerStaticData& operator=(const HoursFieldLexerStaticData&) = delete;
  HoursFieldLexerStaticData& operator=(HoursFieldLexerStaticData&&) = delete;

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

::antlr4::internal::OnceFlag hoursfieldlexerLexerOnceFlag;
#if ANTLR4_USE_THREAD_LOCAL_CACHE
static thread_local
#endif
std::unique_ptr<HoursFieldLexerStaticData> hoursfieldlexerLexerStaticData = nullptr;

void hoursfieldlexerLexerInitialize() {
#if ANTLR4_USE_THREAD_LOCAL_CACHE
  if (hoursfieldlexerLexerStaticData != nullptr) {
    return;
  }
#else
  assert(hoursfieldlexerLexerStaticData == nullptr);
#endif
  auto staticData = std::make_unique<HoursFieldLexerStaticData>(
    std::vector<std::string>{
      "LBRACK", "RBRACK", "COMMA", "INT", "WS"
    },
    std::vector<std::string>{
      "DEFAULT_TOKEN_CHANNEL", "HIDDEN"
    },
    std::vector<std::string>{
      "DEFAULT_MODE"
    },
    std::vector<std::string>{
      "", "'['", "']'", "','"
    },
    std::vector<std::string>{
      "", "LBRACK", "RBRACK", "COMMA", "INT", "WS"
    }
  );
  static const int32_t serializedATNSegment[] = {
  	4,0,5,29,6,-1,2,0,7,0,2,1,7,1,2,2,7,2,2,3,7,3,2,4,7,4,1,0,1,0,1,1,1,1,
  	1,2,1,2,1,3,4,3,19,8,3,11,3,12,3,20,1,4,4,4,24,8,4,11,4,12,4,25,1,4,1,
  	4,0,0,5,1,1,3,2,5,3,7,4,9,5,1,0,2,1,0,48,57,2,0,9,13,32,32,30,0,1,1,0,
  	0,0,0,3,1,0,0,0,0,5,1,0,0,0,0,7,1,0,0,0,0,9,1,0,0,0,1,11,1,0,0,0,3,13,
  	1,0,0,0,5,15,1,0,0,0,7,18,1,0,0,0,9,23,1,0,0,0,11,12,5,91,0,0,12,2,1,
  	0,0,0,13,14,5,93,0,0,14,4,1,0,0,0,15,16,5,44,0,0,16,6,1,0,0,0,17,19,7,
  	0,0,0,18,17,1,0,0,0,19,20,1,0,0,0,20,18,1,0,0,0,20,21,1,0,0,0,21,8,1,
  	0,0,0,22,24,7,1,0,0,23,22,1,0,0,0,24,25,1,0,0,0,25,23,1,0,0,0,25,26,1,
  	0,0,0,26,27,1,0,0,0,27,28,6,4,0,0,28,10,1,0,0,0,3,0,20,25,1,6,0,0
  };
  staticData->serializedATN = antlr4::atn::SerializedATNView(serializedATNSegment, sizeof(serializedATNSegment) / sizeof(serializedATNSegment[0]));

  antlr4::atn::ATNDeserializer deserializer;
  staticData->atn = deserializer.deserialize(staticData->serializedATN);

  const size_t count = staticData->atn->getNumberOfDecisions();
  staticData->decisionToDFA.reserve(count);
  for (size_t i = 0; i < count; i++) { 
    staticData->decisionToDFA.emplace_back(staticData->atn->getDecisionState(i), i);
  }
  hoursfieldlexerLexerStaticData = std::move(staticData);
}

}

HoursFieldLexer::HoursFieldLexer(CharStream *input) : Lexer(input) {
  HoursFieldLexer::initialize();
  _interpreter = new atn::LexerATNSimulator(this, *hoursfieldlexerLexerStaticData->atn, hoursfieldlexerLexerStaticData->decisionToDFA, hoursfieldlexerLexerStaticData->sharedContextCache);
}

HoursFieldLexer::~HoursFieldLexer() {
  delete _interpreter;
}

std::string HoursFieldLexer::getGrammarFileName() const {
  return "HoursField.g4";
}

const std::vector<std::string>& HoursFieldLexer::getRuleNames() const {
  return hoursfieldlexerLexerStaticData->ruleNames;
}

const std::vector<std::string>& HoursFieldLexer::getChannelNames() const {
  return hoursfieldlexerLexerStaticData->channelNames;
}

const std::vector<std::string>& HoursFieldLexer::getModeNames() const {
  return hoursfieldlexerLexerStaticData->modeNames;
}

const dfa::Vocabulary& HoursFieldLexer::getVocabulary() const {
  return hoursfieldlexerLexerStaticData->vocabulary;
}

antlr4::atn::SerializedATNView HoursFieldLexer::getSerializedATN() const {
  return hoursfieldlexerLexerStaticData->serializedATN;
}

const atn::ATN& HoursFieldLexer::getATN() const {
  return *hoursfieldlexerLexerStaticData->atn;
}




void HoursFieldLexer::initialize() {
#if ANTLR4_USE_THREAD_LOCAL_CACHE
  hoursfieldlexerLexerInitialize();
#else
  ::antlr4::internal::call_once(hoursfieldlexerLexerOnceFlag, hoursfieldlexerLexerInitialize);
#endif
}
