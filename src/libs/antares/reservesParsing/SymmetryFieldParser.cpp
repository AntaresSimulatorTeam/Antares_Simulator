
// Generated from SymmetryField.g4 by ANTLR 4.13.2

#include "SymmetryFieldParser.h"

#include "SymmetryFieldListener.h"
#include "SymmetryFieldVisitor.h"

using namespace antlrcpp;

using namespace antlr4;

namespace
{

struct SymmetryFieldParserStaticData final
{
    SymmetryFieldParserStaticData(std::vector<std::string> ruleNames,
                                  std::vector<std::string> literalNames,
                                  std::vector<std::string> symbolicNames):
        ruleNames(std::move(ruleNames)),
        literalNames(std::move(literalNames)),
        symbolicNames(std::move(symbolicNames)),
        vocabulary(this->literalNames, this->symbolicNames)
    {
    }

    SymmetryFieldParserStaticData(const SymmetryFieldParserStaticData&) = delete;
    SymmetryFieldParserStaticData(SymmetryFieldParserStaticData&&) = delete;
    SymmetryFieldParserStaticData& operator=(const SymmetryFieldParserStaticData&) = delete;
    SymmetryFieldParserStaticData& operator=(SymmetryFieldParserStaticData&&) = delete;

    std::vector<antlr4::dfa::DFA> decisionToDFA;
    antlr4::atn::PredictionContextCache sharedContextCache;
    const std::vector<std::string> ruleNames;
    const std::vector<std::string> literalNames;
    const std::vector<std::string> symbolicNames;
    const antlr4::dfa::Vocabulary vocabulary;
    antlr4::atn::SerializedATNView serializedATN;
    std::unique_ptr<antlr4::atn::ATN> atn;
};

::antlr4::internal::OnceFlag symmetryfieldParserOnceFlag;
#if ANTLR4_USE_THREAD_LOCAL_CACHE
static thread_local
#endif
  std::unique_ptr<SymmetryFieldParserStaticData>
    symmetryfieldParserStaticData = nullptr;

void symmetryfieldParserInitialize()
{
#if ANTLR4_USE_THREAD_LOCAL_CACHE
    if (symmetryfieldParserStaticData != nullptr)
    {
        return;
    }
#else
    assert(symmetryfieldParserStaticData == nullptr);
#endif
    auto staticData = std::make_unique<SymmetryFieldParserStaticData>(
      std::vector<std::string>{"symmetryField", "elementList", "element"},
      std::vector<std::string>{"", "','", "'['", "']'"},
      std::vector<std::string>{"", "", "", "", "ID", "WS"});
    static const int32_t serializedATNSegment[] = {
      4,  1,  5,  30, 2,  0,  7,  0,  2,  1,  7,  1,  2,  2,  7,  2,  1,  0,  1,  0,  1,  0,  5,
      0,  10, 8,  0,  10, 0,  12, 0,  13, 9,  0,  1,  0,  1,  0,  1,  1,  1,  1,  1,  1,  1,  1,
      5,  1,  21, 8,  1,  10, 1,  12, 1,  24, 9,  1,  1,  1,  1,  1,  1,  2,  1,  2,  1,  2,  0,
      0,  3,  0,  2,  4,  0,  0,  28, 0,  6,  1,  0,  0,  0,  2,  16, 1,  0,  0,  0,  4,  27, 1,
      0,  0,  0,  6,  11, 3,  2,  1,  0,  7,  8,  5,  1,  0,  0,  8,  10, 3,  2,  1,  0,  9,  7,
      1,  0,  0,  0,  10, 13, 1,  0,  0,  0,  11, 9,  1,  0,  0,  0,  11, 12, 1,  0,  0,  0,  12,
      14, 1,  0,  0,  0,  13, 11, 1,  0,  0,  0,  14, 15, 5,  0,  0,  1,  15, 1,  1,  0,  0,  0,
      16, 17, 5,  2,  0,  0,  17, 22, 3,  4,  2,  0,  18, 19, 5,  1,  0,  0,  19, 21, 3,  4,  2,
      0,  20, 18, 1,  0,  0,  0,  21, 24, 1,  0,  0,  0,  22, 20, 1,  0,  0,  0,  22, 23, 1,  0,
      0,  0,  23, 25, 1,  0,  0,  0,  24, 22, 1,  0,  0,  0,  25, 26, 5,  3,  0,  0,  26, 3,  1,
      0,  0,  0,  27, 28, 5,  4,  0,  0,  28, 5,  1,  0,  0,  0,  2,  11, 22};
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
    symmetryfieldParserStaticData = std::move(staticData);
}

} // namespace

SymmetryFieldParser::SymmetryFieldParser(TokenStream* input):
    SymmetryFieldParser(input, antlr4::atn::ParserATNSimulatorOptions())
{
}

SymmetryFieldParser::SymmetryFieldParser(TokenStream* input,
                                         const antlr4::atn::ParserATNSimulatorOptions& options):
    Parser(input)
{
    SymmetryFieldParser::initialize();
    _interpreter = new atn::ParserATNSimulator(this,
                                               *symmetryfieldParserStaticData->atn,
                                               symmetryfieldParserStaticData->decisionToDFA,
                                               symmetryfieldParserStaticData->sharedContextCache,
                                               options);
}

SymmetryFieldParser::~SymmetryFieldParser()
{
    delete _interpreter;
}

const atn::ATN& SymmetryFieldParser::getATN() const
{
    return *symmetryfieldParserStaticData->atn;
}

std::string SymmetryFieldParser::getGrammarFileName() const
{
    return "SymmetryField.g4";
}

const std::vector<std::string>& SymmetryFieldParser::getRuleNames() const
{
    return symmetryfieldParserStaticData->ruleNames;
}

const dfa::Vocabulary& SymmetryFieldParser::getVocabulary() const
{
    return symmetryfieldParserStaticData->vocabulary;
}

antlr4::atn::SerializedATNView SymmetryFieldParser::getSerializedATN() const
{
    return symmetryfieldParserStaticData->serializedATN;
}

//----------------- SymmetryFieldContext
//------------------------------------------------------------------

SymmetryFieldParser::SymmetryFieldContext::SymmetryFieldContext(ParserRuleContext* parent,
                                                                size_t invokingState):
    ParserRuleContext(parent, invokingState)
{
}

std::vector<SymmetryFieldParser::ElementListContext*>
SymmetryFieldParser::SymmetryFieldContext::elementList()
{
    return getRuleContexts<SymmetryFieldParser::ElementListContext>();
}

SymmetryFieldParser::ElementListContext* SymmetryFieldParser::SymmetryFieldContext::elementList(
  size_t i)
{
    return getRuleContext<SymmetryFieldParser::ElementListContext>(i);
}

tree::TerminalNode* SymmetryFieldParser::SymmetryFieldContext::EOF()
{
    return getToken(SymmetryFieldParser::EOF, 0);
}

size_t SymmetryFieldParser::SymmetryFieldContext::getRuleIndex() const
{
    return SymmetryFieldParser::RuleSymmetryField;
}

void SymmetryFieldParser::SymmetryFieldContext::enterRule(tree::ParseTreeListener* listener)
{
    auto parserListener = dynamic_cast<SymmetryFieldListener*>(listener);
    if (parserListener != nullptr)
    {
        parserListener->enterSymmetryField(this);
    }
}

void SymmetryFieldParser::SymmetryFieldContext::exitRule(tree::ParseTreeListener* listener)
{
    auto parserListener = dynamic_cast<SymmetryFieldListener*>(listener);
    if (parserListener != nullptr)
    {
        parserListener->exitSymmetryField(this);
    }
}

std::any SymmetryFieldParser::SymmetryFieldContext::accept(tree::ParseTreeVisitor* visitor)
{
    if (auto parserVisitor = dynamic_cast<SymmetryFieldVisitor*>(visitor))
    {
        return parserVisitor->visitSymmetryField(this);
    }
    else
    {
        return visitor->visitChildren(this);
    }
}

SymmetryFieldParser::SymmetryFieldContext* SymmetryFieldParser::symmetryField()
{
    SymmetryFieldContext* _localctx = _tracker.createInstance<SymmetryFieldContext>(_ctx,
                                                                                    getState());
    enterRule(_localctx, 0, SymmetryFieldParser::RuleSymmetryField);
    size_t _la = 0;

#if __cplusplus > 201703L
    auto onExit = finally(
      [=, this]
      {
#else
    auto onExit = finally(
      [=]
      {
#endif
          exitRule();
      });
    try
    {
        enterOuterAlt(_localctx, 1);
        setState(6);
        elementList();
        setState(11);
        _errHandler->sync(this);
        _la = _input->LA(1);
        while (_la == SymmetryFieldParser::T__0)
        {
            setState(7);
            match(SymmetryFieldParser::T__0);
            setState(8);
            elementList();
            setState(13);
            _errHandler->sync(this);
            _la = _input->LA(1);
        }
        setState(14);
        match(SymmetryFieldParser::EOF);
    }
    catch (RecognitionException& e)
    {
        _errHandler->reportError(this, e);
        _localctx->exception = std::current_exception();
        _errHandler->recover(this, _localctx->exception);
    }

    return _localctx;
}

//----------------- ElementListContext
//------------------------------------------------------------------

SymmetryFieldParser::ElementListContext::ElementListContext(ParserRuleContext* parent,
                                                            size_t invokingState):
    ParserRuleContext(parent, invokingState)
{
}

std::vector<SymmetryFieldParser::ElementContext*> SymmetryFieldParser::ElementListContext::element()
{
    return getRuleContexts<SymmetryFieldParser::ElementContext>();
}

SymmetryFieldParser::ElementContext* SymmetryFieldParser::ElementListContext::element(size_t i)
{
    return getRuleContext<SymmetryFieldParser::ElementContext>(i);
}

size_t SymmetryFieldParser::ElementListContext::getRuleIndex() const
{
    return SymmetryFieldParser::RuleElementList;
}

void SymmetryFieldParser::ElementListContext::enterRule(tree::ParseTreeListener* listener)
{
    auto parserListener = dynamic_cast<SymmetryFieldListener*>(listener);
    if (parserListener != nullptr)
    {
        parserListener->enterElementList(this);
    }
}

void SymmetryFieldParser::ElementListContext::exitRule(tree::ParseTreeListener* listener)
{
    auto parserListener = dynamic_cast<SymmetryFieldListener*>(listener);
    if (parserListener != nullptr)
    {
        parserListener->exitElementList(this);
    }
}

std::any SymmetryFieldParser::ElementListContext::accept(tree::ParseTreeVisitor* visitor)
{
    if (auto parserVisitor = dynamic_cast<SymmetryFieldVisitor*>(visitor))
    {
        return parserVisitor->visitElementList(this);
    }
    else
    {
        return visitor->visitChildren(this);
    }
}

SymmetryFieldParser::ElementListContext* SymmetryFieldParser::elementList()
{
    ElementListContext* _localctx = _tracker.createInstance<ElementListContext>(_ctx, getState());
    enterRule(_localctx, 2, SymmetryFieldParser::RuleElementList);
    size_t _la = 0;

#if __cplusplus > 201703L
    auto onExit = finally(
      [=, this]
      {
#else
    auto onExit = finally(
      [=]
      {
#endif
          exitRule();
      });
    try
    {
        enterOuterAlt(_localctx, 1);
        setState(16);
        match(SymmetryFieldParser::T__1);
        setState(17);
        element();
        setState(22);
        _errHandler->sync(this);
        _la = _input->LA(1);
        while (_la == SymmetryFieldParser::T__0)
        {
            setState(18);
            match(SymmetryFieldParser::T__0);
            setState(19);
            element();
            setState(24);
            _errHandler->sync(this);
            _la = _input->LA(1);
        }
        setState(25);
        match(SymmetryFieldParser::T__2);
    }
    catch (RecognitionException& e)
    {
        _errHandler->reportError(this, e);
        _localctx->exception = std::current_exception();
        _errHandler->recover(this, _localctx->exception);
    }

    return _localctx;
}

//----------------- ElementContext
//------------------------------------------------------------------

SymmetryFieldParser::ElementContext::ElementContext(ParserRuleContext* parent,
                                                    size_t invokingState):
    ParserRuleContext(parent, invokingState)
{
}

tree::TerminalNode* SymmetryFieldParser::ElementContext::ID()
{
    return getToken(SymmetryFieldParser::ID, 0);
}

size_t SymmetryFieldParser::ElementContext::getRuleIndex() const
{
    return SymmetryFieldParser::RuleElement;
}

void SymmetryFieldParser::ElementContext::enterRule(tree::ParseTreeListener* listener)
{
    auto parserListener = dynamic_cast<SymmetryFieldListener*>(listener);
    if (parserListener != nullptr)
    {
        parserListener->enterElement(this);
    }
}

void SymmetryFieldParser::ElementContext::exitRule(tree::ParseTreeListener* listener)
{
    auto parserListener = dynamic_cast<SymmetryFieldListener*>(listener);
    if (parserListener != nullptr)
    {
        parserListener->exitElement(this);
    }
}

std::any SymmetryFieldParser::ElementContext::accept(tree::ParseTreeVisitor* visitor)
{
    if (auto parserVisitor = dynamic_cast<SymmetryFieldVisitor*>(visitor))
    {
        return parserVisitor->visitElement(this);
    }
    else
    {
        return visitor->visitChildren(this);
    }
}

SymmetryFieldParser::ElementContext* SymmetryFieldParser::element()
{
    ElementContext* _localctx = _tracker.createInstance<ElementContext>(_ctx, getState());
    enterRule(_localctx, 4, SymmetryFieldParser::RuleElement);

#if __cplusplus > 201703L
    auto onExit = finally(
      [=, this]
      {
#else
    auto onExit = finally(
      [=]
      {
#endif
          exitRule();
      });
    try
    {
        enterOuterAlt(_localctx, 1);
        setState(27);
        match(SymmetryFieldParser::ID);
    }
    catch (RecognitionException& e)
    {
        _errHandler->reportError(this, e);
        _localctx->exception = std::current_exception();
        _errHandler->recover(this, _localctx->exception);
    }

    return _localctx;
}

void SymmetryFieldParser::initialize()
{
#if ANTLR4_USE_THREAD_LOCAL_CACHE
    symmetryfieldParserInitialize();
#else
    ::antlr4::internal::call_once(symmetryfieldParserOnceFlag, symmetryfieldParserInitialize);
#endif
}
