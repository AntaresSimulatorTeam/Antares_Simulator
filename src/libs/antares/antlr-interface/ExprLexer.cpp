
// Generated from Expr.g4 by ANTLR 4.13.2


#include "ExprLexer.h"


using namespace antlr4;



using namespace antlr4;

namespace {

struct ExprLexerStaticData final {
  ExprLexerStaticData(std::vector<std::string> ruleNames,
                          std::vector<std::string> channelNames,
                          std::vector<std::string> modeNames,
                          std::vector<std::string> literalNames,
                          std::vector<std::string> symbolicNames)
      : ruleNames(std::move(ruleNames)), channelNames(std::move(channelNames)),
        modeNames(std::move(modeNames)), literalNames(std::move(literalNames)),
        symbolicNames(std::move(symbolicNames)),
        vocabulary(this->literalNames, this->symbolicNames) {}

  ExprLexerStaticData(const ExprLexerStaticData&) = delete;
  ExprLexerStaticData(ExprLexerStaticData&&) = delete;
  ExprLexerStaticData& operator=(const ExprLexerStaticData&) = delete;
  ExprLexerStaticData& operator=(ExprLexerStaticData&&) = delete;

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

::antlr4::internal::OnceFlag exprlexerLexerOnceFlag;
#if ANTLR4_USE_THREAD_LOCAL_CACHE
static thread_local
#endif
std::unique_ptr<ExprLexerStaticData> exprlexerLexerStaticData = nullptr;

void exprlexerLexerInitialize() {
#if ANTLR4_USE_THREAD_LOCAL_CACHE
  if (exprlexerLexerStaticData != nullptr) {
    return;
  }
#else
  assert(exprlexerLexerStaticData == nullptr);
#endif
  auto staticData = std::make_unique<ExprLexerStaticData>(
    std::vector<std::string>{
      "T__0", "T__1", "T__2", "T__3", "T__4", "T__5", "T__6", "T__7", "T__8", 
      "T__9", "T__10", "T__11", "DIGIT", "CHAR", "CHAR_OR_DIGIT", "NUMBER", 
      "TIME", "IDENTIFIER", "COMPARISON", "WS"
    },
    std::vector<std::string>{
      "DEFAULT_TOKEN_CHANNEL", "HIDDEN"
    },
    std::vector<std::string>{
      "DEFAULT_MODE"
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
  	4,0,17,109,6,-1,2,0,7,0,2,1,7,1,2,2,7,2,2,3,7,3,2,4,7,4,2,5,7,5,2,6,7,
  	6,2,7,7,7,2,8,7,8,2,9,7,9,2,10,7,10,2,11,7,11,2,12,7,12,2,13,7,13,2,14,
  	7,14,2,15,7,15,2,16,7,16,2,17,7,17,2,18,7,18,2,19,7,19,1,0,1,0,1,1,1,
  	1,1,2,1,2,1,3,1,3,1,4,1,4,1,5,1,5,1,6,1,6,1,7,1,7,1,7,1,7,1,8,1,8,1,8,
  	1,9,1,9,1,10,1,10,1,11,1,11,1,12,1,12,1,13,1,13,1,14,1,14,3,14,75,8,14,
  	1,15,4,15,78,8,15,11,15,12,15,79,1,15,1,15,4,15,84,8,15,11,15,12,15,85,
  	3,15,88,8,15,1,16,1,16,1,17,1,17,5,17,94,8,17,10,17,12,17,97,9,17,1,18,
  	1,18,1,18,1,18,1,18,3,18,104,8,18,1,19,1,19,1,19,1,19,0,0,20,1,1,3,2,
  	5,3,7,4,9,5,11,6,13,7,15,8,17,9,19,10,21,11,23,12,25,0,27,0,29,0,31,13,
  	33,14,35,15,37,16,39,17,1,0,3,1,0,48,57,3,0,65,90,95,95,97,122,3,0,9,
  	10,13,13,32,32,112,0,1,1,0,0,0,0,3,1,0,0,0,0,5,1,0,0,0,0,7,1,0,0,0,0,
  	9,1,0,0,0,0,11,1,0,0,0,0,13,1,0,0,0,0,15,1,0,0,0,0,17,1,0,0,0,0,19,1,
  	0,0,0,0,21,1,0,0,0,0,23,1,0,0,0,0,31,1,0,0,0,0,33,1,0,0,0,0,35,1,0,0,
  	0,0,37,1,0,0,0,0,39,1,0,0,0,1,41,1,0,0,0,3,43,1,0,0,0,5,45,1,0,0,0,7,
  	47,1,0,0,0,9,49,1,0,0,0,11,51,1,0,0,0,13,53,1,0,0,0,15,55,1,0,0,0,17,
  	59,1,0,0,0,19,62,1,0,0,0,21,64,1,0,0,0,23,66,1,0,0,0,25,68,1,0,0,0,27,
  	70,1,0,0,0,29,74,1,0,0,0,31,77,1,0,0,0,33,89,1,0,0,0,35,91,1,0,0,0,37,
  	103,1,0,0,0,39,105,1,0,0,0,41,42,5,46,0,0,42,2,1,0,0,0,43,44,5,45,0,0,
  	44,4,1,0,0,0,45,46,5,40,0,0,46,6,1,0,0,0,47,48,5,41,0,0,48,8,1,0,0,0,
  	49,50,5,47,0,0,50,10,1,0,0,0,51,52,5,42,0,0,52,12,1,0,0,0,53,54,5,43,
  	0,0,54,14,1,0,0,0,55,56,5,115,0,0,56,57,5,117,0,0,57,58,5,109,0,0,58,
  	16,1,0,0,0,59,60,5,46,0,0,60,61,5,46,0,0,61,18,1,0,0,0,62,63,5,44,0,0,
  	63,20,1,0,0,0,64,65,5,91,0,0,65,22,1,0,0,0,66,67,5,93,0,0,67,24,1,0,0,
  	0,68,69,7,0,0,0,69,26,1,0,0,0,70,71,7,1,0,0,71,28,1,0,0,0,72,75,3,27,
  	13,0,73,75,3,25,12,0,74,72,1,0,0,0,74,73,1,0,0,0,75,30,1,0,0,0,76,78,
  	3,25,12,0,77,76,1,0,0,0,78,79,1,0,0,0,79,77,1,0,0,0,79,80,1,0,0,0,80,
  	87,1,0,0,0,81,83,5,46,0,0,82,84,3,25,12,0,83,82,1,0,0,0,84,85,1,0,0,0,
  	85,83,1,0,0,0,85,86,1,0,0,0,86,88,1,0,0,0,87,81,1,0,0,0,87,88,1,0,0,0,
  	88,32,1,0,0,0,89,90,5,116,0,0,90,34,1,0,0,0,91,95,3,27,13,0,92,94,3,29,
  	14,0,93,92,1,0,0,0,94,97,1,0,0,0,95,93,1,0,0,0,95,96,1,0,0,0,96,36,1,
  	0,0,0,97,95,1,0,0,0,98,104,5,61,0,0,99,100,5,62,0,0,100,104,5,61,0,0,
  	101,102,5,60,0,0,102,104,5,61,0,0,103,98,1,0,0,0,103,99,1,0,0,0,103,101,
  	1,0,0,0,104,38,1,0,0,0,105,106,7,2,0,0,106,107,1,0,0,0,107,108,6,19,0,
  	0,108,40,1,0,0,0,7,0,74,79,85,87,95,103,1,6,0,0
  };
  staticData->serializedATN = antlr4::atn::SerializedATNView(serializedATNSegment, sizeof(serializedATNSegment) / sizeof(serializedATNSegment[0]));

  antlr4::atn::ATNDeserializer deserializer;
  staticData->atn = deserializer.deserialize(staticData->serializedATN);

  const size_t count = staticData->atn->getNumberOfDecisions();
  staticData->decisionToDFA.reserve(count);
  for (size_t i = 0; i < count; i++) { 
    staticData->decisionToDFA.emplace_back(staticData->atn->getDecisionState(i), i);
  }
  exprlexerLexerStaticData = std::move(staticData);
}

}

ExprLexer::ExprLexer(CharStream *input) : Lexer(input) {
  ExprLexer::initialize();
  _interpreter = new atn::LexerATNSimulator(this, *exprlexerLexerStaticData->atn, exprlexerLexerStaticData->decisionToDFA, exprlexerLexerStaticData->sharedContextCache);
}

ExprLexer::~ExprLexer() {
  delete _interpreter;
}

std::string ExprLexer::getGrammarFileName() const {
  return "Expr.g4";
}

const std::vector<std::string>& ExprLexer::getRuleNames() const {
  return exprlexerLexerStaticData->ruleNames;
}

const std::vector<std::string>& ExprLexer::getChannelNames() const {
  return exprlexerLexerStaticData->channelNames;
}

const std::vector<std::string>& ExprLexer::getModeNames() const {
  return exprlexerLexerStaticData->modeNames;
}

const dfa::Vocabulary& ExprLexer::getVocabulary() const {
  return exprlexerLexerStaticData->vocabulary;
}

antlr4::atn::SerializedATNView ExprLexer::getSerializedATN() const {
  return exprlexerLexerStaticData->serializedATN;
}

const atn::ATN& ExprLexer::getATN() const {
  return *exprlexerLexerStaticData->atn;
}




void ExprLexer::initialize() {
#if ANTLR4_USE_THREAD_LOCAL_CACHE
  exprlexerLexerInitialize();
#else
  ::antlr4::internal::call_once(exprlexerLexerOnceFlag, exprlexerLexerInitialize);
#endif
}
