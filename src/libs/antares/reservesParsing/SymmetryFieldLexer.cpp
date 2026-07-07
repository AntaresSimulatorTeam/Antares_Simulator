
// Generated from SymmetryField.g4 by ANTLR 4.13.2

#include "SymmetryFieldLexer.h"

using namespace antlr4;

using namespace antlr4;

namespace
{

struct SymmetryFieldLexerStaticData final
{
    SymmetryFieldLexerStaticData(std::vector<std::string> ruleNames,
                                 std::vector<std::string> channelNames,
                                 std::vector<std::string> modeNames,
                                 std::vector<std::string> literalNames,
                                 std::vector<std::string> symbolicNames):
        ruleNames(std::move(ruleNames)),
        channelNames(std::move(channelNames)),
        modeNames(std::move(modeNames)),
        literalNames(std::move(literalNames)),
        symbolicNames(std::move(symbolicNames)),
        vocabulary(this->literalNames, this->symbolicNames)
    {
    }

    SymmetryFieldLexerStaticData(const SymmetryFieldLexerStaticData&) = delete;
    SymmetryFieldLexerStaticData(SymmetryFieldLexerStaticData&&) = delete;
    SymmetryFieldLexerStaticData& operator=(const SymmetryFieldLexerStaticData&) = delete;
    SymmetryFieldLexerStaticData& operator=(SymmetryFieldLexerStaticData&&) = delete;

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

::antlr4::internal::OnceFlag symmetryfieldlexerLexerOnceFlag;
#if ANTLR4_USE_THREAD_LOCAL_CACHE
static thread_local
#endif
  std::unique_ptr<SymmetryFieldLexerStaticData>
    symmetryfieldlexerLexerStaticData = nullptr;

void symmetryfieldlexerLexerInitialize()
{
#if ANTLR4_USE_THREAD_LOCAL_CACHE
    if (symmetryfieldlexerLexerStaticData != nullptr)
    {
        return;
    }
#else
    assert(symmetryfieldlexerLexerStaticData == nullptr);
#endif
    auto staticData = std::make_unique<SymmetryFieldLexerStaticData>(
      std::vector<std::string>{"T__0", "T__1", "T__2", "ID", "WS"},
      std::vector<std::string>{"DEFAULT_TOKEN_CHANNEL", "HIDDEN"},
      std::vector<std::string>{"DEFAULT_MODE"},
      std::vector<std::string>{"", "','", "'['", "']'"},
      std::vector<std::string>{"", "", "", "", "ID", "WS"});
    static const int32_t serializedATNSegment[] = {
      4,  0,  5,  29, 6,  -1, 2,  0, 7, 0,  2,  1,  7,  1,  2,  2,  7,  2,  2,  3,   7, 3,  2,  4,
      7,  4,  1,  0,  1,  0,  1,  1, 1, 1,  1,  2,  1,  2,  1,  3,  4,  3,  19, 8,   3, 11, 3,  12,
      3,  20, 1,  4,  4,  4,  24, 8, 4, 11, 4,  12, 4,  25, 1,  4,  1,  4,  0,  0,   5, 1,  1,  3,
      2,  5,  3,  7,  4,  9,  5,  1, 0, 2,  4,  0,  48, 57, 65, 90, 95, 95, 97, 122, 2, 0,  9,  13,
      32, 32, 30, 0,  1,  1,  0,  0, 0, 0,  3,  1,  0,  0,  0,  0,  5,  1,  0,  0,   0, 0,  7,  1,
      0,  0,  0,  0,  9,  1,  0,  0, 0, 1,  11, 1,  0,  0,  0,  3,  13, 1,  0,  0,   0, 5,  15, 1,
      0,  0,  0,  7,  18, 1,  0,  0, 0, 9,  23, 1,  0,  0,  0,  11, 12, 5,  44, 0,   0, 12, 2,  1,
      0,  0,  0,  13, 14, 5,  91, 0, 0, 14, 4,  1,  0,  0,  0,  15, 16, 5,  93, 0,   0, 16, 6,  1,
      0,  0,  0,  17, 19, 7,  0,  0, 0, 18, 17, 1,  0,  0,  0,  19, 20, 1,  0,  0,   0, 20, 18, 1,
      0,  0,  0,  20, 21, 1,  0,  0, 0, 21, 8,  1,  0,  0,  0,  22, 24, 7,  1,  0,   0, 23, 22, 1,
      0,  0,  0,  24, 25, 1,  0,  0, 0, 25, 23, 1,  0,  0,  0,  25, 26, 1,  0,  0,   0, 26, 27, 1,
      0,  0,  0,  27, 28, 6,  4,  0, 0, 28, 10, 1,  0,  0,  0,  3,  0,  20, 25, 1,   6, 0,  0};
    staticData->serializedATN = antlr4::atn::SerializedATNView(serializedATNSegment,
                                                               sizeof(serializedATNSegment)
                                                                 / sizeof(serializedATNSegment[0]));

    antlr4::atn::ATNDeserializer deserializer;
    staticData->atn = deserializer.deserialize(staticData->serializedATN);

    const size_t count = staticData->atn->getNumberOfDecisions();
    staticData->decisionToDFA.reserve(count);
    for (size_t i = 0; i < count; i++)
    {
        staticData->decisionToDFA.emplace_back(staticData->atn->getDecisionState(i), i);
    }
    symmetryfieldlexerLexerStaticData = std::move(staticData);
}

} // namespace

SymmetryFieldLexer::SymmetryFieldLexer(CharStream* input):
    Lexer(input)
{
    SymmetryFieldLexer::initialize();
    _interpreter = new atn::LexerATNSimulator(
      this,
      *symmetryfieldlexerLexerStaticData->atn,
      symmetryfieldlexerLexerStaticData->decisionToDFA,
      symmetryfieldlexerLexerStaticData->sharedContextCache);
}

SymmetryFieldLexer::~SymmetryFieldLexer()
{
    delete _interpreter;
}

std::string SymmetryFieldLexer::getGrammarFileName() const
{
    return "SymmetryField.g4";
}

const std::vector<std::string>& SymmetryFieldLexer::getRuleNames() const
{
    return symmetryfieldlexerLexerStaticData->ruleNames;
}

const std::vector<std::string>& SymmetryFieldLexer::getChannelNames() const
{
    return symmetryfieldlexerLexerStaticData->channelNames;
}

const std::vector<std::string>& SymmetryFieldLexer::getModeNames() const
{
    return symmetryfieldlexerLexerStaticData->modeNames;
}

const dfa::Vocabulary& SymmetryFieldLexer::getVocabulary() const
{
    return symmetryfieldlexerLexerStaticData->vocabulary;
}

antlr4::atn::SerializedATNView SymmetryFieldLexer::getSerializedATN() const
{
    return symmetryfieldlexerLexerStaticData->serializedATN;
}

const atn::ATN& SymmetryFieldLexer::getATN() const
{
    return *symmetryfieldlexerLexerStaticData->atn;
}

void SymmetryFieldLexer::initialize()
{
#if ANTLR4_USE_THREAD_LOCAL_CACHE
    symmetryfieldlexerLexerInitialize();
#else
    ::antlr4::internal::call_once(symmetryfieldlexerLexerOnceFlag,
                                  symmetryfieldlexerLexerInitialize);
#endif
}
