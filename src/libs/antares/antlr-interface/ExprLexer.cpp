
// Generated from Expr.g4 by ANTLR 4.13.1

#include "ExprLexer.h"

using namespace antlr4;

using namespace antlr4;

namespace
{

struct ExprLexerStaticData final
{
    ExprLexerStaticData(std::vector<std::string> ruleNames,
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
  ExprLexerStaticData* exprlexerLexerStaticData
  = nullptr;

void exprlexerLexerInitialize()
{
#if ANTLR4_USE_THREAD_LOCAL_CACHE
    if (exprlexerLexerStaticData != nullptr)
    {
        return;
    }
#else
    assert(exprlexerLexerStaticData == nullptr);
#endif
    auto staticData = std::make_unique<ExprLexerStaticData>(
      std::vector<std::string>{"T__0",       "T__1",          "T__2",   "T__3",     "T__4",
                               "T__5",       "T__6",          "T__7",   "T__8",     "DIGIT",
                               "CHAR",       "CHAR_OR_DIGIT", "NUMBER", "TIME",     "IDENTIFIER",
                               "COMPARISON", "ADDSUB",        "MULDIV", "LBRACKET", "RBRACKET",
                               "WS"},
      std::vector<std::string>{"DEFAULT_TOKEN_CHANNEL", "HIDDEN"},
      std::vector<std::string>{"DEFAULT_MODE"},
      std::vector<std::string>{"",
                               "'+'",
                               "'-'",
                               "'/'",
                               "'*'",
                               "'.'",
                               "'('",
                               "')'",
                               "','",
                               "'..'",
                               "",
                               "'t'",
                               "",
                               "",
                               "",
                               "",
                               "'['",
                               "']'"},
      std::vector<std::string>{"",
                               "",
                               "",
                               "",
                               "",
                               "",
                               "",
                               "",
                               "",
                               "",
                               "NUMBER",
                               "TIME",
                               "IDENTIFIER",
                               "COMPARISON",
                               "ADDSUB",
                               "MULDIV",
                               "LBRACKET",
                               "RBRACKET",
                               "WS"});
    static const int32_t serializedATNSegment[] = {
      4,   0,   18,  111, 6,  -1, 2,   0,   7,   0,   2,   1,   7,  1,   2,   2,   7,  2,   2,
      3,   7,   3,   2,   4,  7,  4,   2,   5,   7,   5,   2,   6,  7,   6,   2,   7,  7,   7,
      2,   8,   7,   8,   2,  9,  7,   9,   2,   10,  7,   10,  2,  11,  7,   11,  2,  12,  7,
      12,  2,   13,  7,   13, 2,  14,  7,   14,  2,   15,  7,   15, 2,   16,  7,   16, 2,   17,
      7,   17,  2,   18,  7,  18, 2,   19,  7,   19,  2,   20,  7,  20,  1,   0,   1,  0,   1,
      1,   1,   1,   1,   2,  1,  2,   1,   3,   1,   3,   1,   4,  1,   4,   1,   5,  1,   5,
      1,   6,   1,   6,   1,  7,  1,   7,   1,   8,   1,   8,   1,  8,   1,   9,   1,  9,   1,
      10,  1,   10,  1,   11, 1,  11,  3,   11,  69,  8,   11,  1,  12,  4,   12,  72, 8,   12,
      11,  12,  12,  12,  73, 1,  12,  1,   12,  4,   12,  78,  8,  12,  11,  12,  12, 12,  79,
      3,   12,  82,  8,   12, 1,  13,  1,   13,  1,   14,  1,   14, 5,   14,  88,  8,  14,  10,
      14,  12,  14,  91,  9,  14, 1,   15,  1,   15,  1,   15,  1,  15,  1,   15,  3,  15,  98,
      8,   15,  1,   16,  1,  16, 1,   17,  1,   17,  1,   18,  1,  18,  1,   19,  1,  19,  1,
      20,  1,   20,  1,   20, 1,  20,  0,   0,   21,  1,   1,   3,  2,   5,   3,   7,  4,   9,
      5,   11,  6,   13,  7,  15, 8,   17,  9,   19,  0,   21,  0,  23,  0,   25,  10, 27,  11,
      29,  12,  31,  13,  33, 14, 35,  15,  37,  16,  39,  17,  41, 18,  1,   0,   5,  1,   0,
      48,  57,  3,   0,   65, 90, 95,  95,  97,  122, 2,   0,   43, 43,  45,  45,  2,  0,   42,
      42,  47,  47,  3,   0,  9,  10,  13,  13,  32,  32,  114, 0,  1,   1,   0,   0,  0,   0,
      3,   1,   0,   0,   0,  0,  5,   1,   0,   0,   0,   0,   7,  1,   0,   0,   0,  0,   9,
      1,   0,   0,   0,   0,  11, 1,   0,   0,   0,   0,   13,  1,  0,   0,   0,   0,  15,  1,
      0,   0,   0,   0,   17, 1,  0,   0,   0,   0,   25,  1,   0,  0,   0,   0,   27, 1,   0,
      0,   0,   0,   29,  1,  0,  0,   0,   0,   31,  1,   0,   0,  0,   0,   33,  1,  0,   0,
      0,   0,   35,  1,   0,  0,  0,   0,   37,  1,   0,   0,   0,  0,   39,  1,   0,  0,   0,
      0,   41,  1,   0,   0,  0,  1,   43,  1,   0,   0,   0,   3,  45,  1,   0,   0,  0,   5,
      47,  1,   0,   0,   0,  7,  49,  1,   0,   0,   0,   9,   51, 1,   0,   0,   0,  11,  53,
      1,   0,   0,   0,   13, 55, 1,   0,   0,   0,   15,  57,  1,  0,   0,   0,   17, 59,  1,
      0,   0,   0,   19,  62, 1,  0,   0,   0,   21,  64,  1,   0,  0,   0,   23,  68, 1,   0,
      0,   0,   25,  71,  1,  0,  0,   0,   27,  83,  1,   0,   0,  0,   29,  85,  1,  0,   0,
      0,   31,  97,  1,   0,  0,  0,   33,  99,  1,   0,   0,   0,  35,  101, 1,   0,  0,   0,
      37,  103, 1,   0,   0,  0,  39,  105, 1,   0,   0,   0,   41, 107, 1,   0,   0,  0,   43,
      44,  5,   43,  0,   0,  44, 2,   1,   0,   0,   0,   45,  46, 5,   45,  0,   0,  46,  4,
      1,   0,   0,   0,   47, 48, 5,   47,  0,   0,   48,  6,   1,  0,   0,   0,   49, 50,  5,
      42,  0,   0,   50,  8,  1,  0,   0,   0,   51,  52,  5,   46, 0,   0,   52,  10, 1,   0,
      0,   0,   53,  54,  5,  40, 0,   0,   54,  12,  1,   0,   0,  0,   55,  56,  5,  41,  0,
      0,   56,  14,  1,   0,  0,  0,   57,  58,  5,   44,  0,   0,  58,  16,  1,   0,  0,   0,
      59,  60,  5,   46,  0,  0,  60,  61,  5,   46,  0,   0,   61, 18,  1,   0,   0,  0,   62,
      63,  7,   0,   0,   0,  63, 20,  1,   0,   0,   0,   64,  65, 7,   1,   0,   0,  65,  22,
      1,   0,   0,   0,   66, 69, 3,   21,  10,  0,   67,  69,  3,  19,  9,   0,   68, 66,  1,
      0,   0,   0,   68,  67, 1,  0,   0,   0,   69,  24,  1,   0,  0,   0,   70,  72, 3,   19,
      9,   0,   71,  70,  1,  0,  0,   0,   72,  73,  1,   0,   0,  0,   73,  71,  1,  0,   0,
      0,   73,  74,  1,   0,  0,  0,   74,  81,  1,   0,   0,   0,  75,  77,  5,   46, 0,   0,
      76,  78,  3,   19,  9,  0,  77,  76,  1,   0,   0,   0,   78, 79,  1,   0,   0,  0,   79,
      77,  1,   0,   0,   0,  79, 80,  1,   0,   0,   0,   80,  82, 1,   0,   0,   0,  81,  75,
      1,   0,   0,   0,   81, 82, 1,   0,   0,   0,   82,  26,  1,  0,   0,   0,   83, 84,  5,
      116, 0,   0,   84,  28, 1,  0,   0,   0,   85,  89,  3,   21, 10,  0,   86,  88, 3,   23,
      11,  0,   87,  86,  1,  0,  0,   0,   88,  91,  1,   0,   0,  0,   89,  87,  1,  0,   0,
      0,   89,  90,  1,   0,  0,  0,   90,  30,  1,   0,   0,   0,  91,  89,  1,   0,  0,   0,
      92,  98,  5,   61,  0,  0,  93,  94,  5,   62,  0,   0,   94, 98,  5,   61,  0,  0,   95,
      96,  5,   60,  0,   0,  96, 98,  5,   61,  0,   0,   97,  92, 1,   0,   0,   0,  97,  93,
      1,   0,   0,   0,   97, 95, 1,   0,   0,   0,   98,  32,  1,  0,   0,   0,   99, 100, 7,
      2,   0,   0,   100, 34, 1,  0,   0,   0,   101, 102, 7,   3,  0,   0,   102, 36, 1,   0,
      0,   0,   103, 104, 5,  91, 0,   0,   104, 38,  1,   0,   0,  0,   105, 106, 5,  93,  0,
      0,   106, 40,  1,   0,  0,  0,   107, 108, 7,   4,   0,   0,  108, 109, 1,   0,  0,   0,
      109, 110, 6,   20,  0,  0,  110, 42,  1,   0,   0,   0,   7,  0,   68,  73,  79, 81,  89,
      97,  1,   6,   0,   0};
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
    exprlexerLexerStaticData = staticData.release();
}

} // namespace

ExprLexer::ExprLexer(CharStream* input):
    Lexer(input)
{
    ExprLexer::initialize();
    _interpreter = new atn::LexerATNSimulator(this,
                                              *exprlexerLexerStaticData->atn,
                                              exprlexerLexerStaticData->decisionToDFA,
                                              exprlexerLexerStaticData->sharedContextCache);
}

ExprLexer::~ExprLexer()
{
    delete _interpreter;
}

std::string ExprLexer::getGrammarFileName() const
{
    return "Expr.g4";
}

const std::vector<std::string>& ExprLexer::getRuleNames() const
{
    return exprlexerLexerStaticData->ruleNames;
}

const std::vector<std::string>& ExprLexer::getChannelNames() const
{
    return exprlexerLexerStaticData->channelNames;
}

const std::vector<std::string>& ExprLexer::getModeNames() const
{
    return exprlexerLexerStaticData->modeNames;
}

const dfa::Vocabulary& ExprLexer::getVocabulary() const
{
    return exprlexerLexerStaticData->vocabulary;
}

antlr4::atn::SerializedATNView ExprLexer::getSerializedATN() const
{
    return exprlexerLexerStaticData->serializedATN;
}

const atn::ATN& ExprLexer::getATN() const
{
    return *exprlexerLexerStaticData->atn;
}

void ExprLexer::initialize()
{
#if ANTLR4_USE_THREAD_LOCAL_CACHE
    exprlexerLexerInitialize();
#else
    ::antlr4::internal::call_once(exprlexerLexerOnceFlag, exprlexerLexerInitialize);
#endif
}
