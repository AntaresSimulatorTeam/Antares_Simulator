
// Generated from SymmetryField.g4 by ANTLR 4.13.2

#pragma once

#include "antlr4-runtime.h"

class SymmetryFieldParser: public antlr4::Parser
{
public:
    enum
    {
        T__0 = 1,
        T__1 = 2,
        T__2 = 3,
        ID = 4,
        WS = 5
    };

    enum
    {
        RuleSymmetryField = 0,
        RuleElementList = 1,
        RuleElement = 2
    };

    explicit SymmetryFieldParser(antlr4::TokenStream* input);

    SymmetryFieldParser(antlr4::TokenStream* input,
                        const antlr4::atn::ParserATNSimulatorOptions& options);

    ~SymmetryFieldParser() override;

    std::string getGrammarFileName() const override;

    const antlr4::atn::ATN& getATN() const override;

    const std::vector<std::string>& getRuleNames() const override;

    const antlr4::dfa::Vocabulary& getVocabulary() const override;

    antlr4::atn::SerializedATNView getSerializedATN() const override;

    class SymmetryFieldContext;
    class ElementListContext;
    class ElementContext;

    class SymmetryFieldContext: public antlr4::ParserRuleContext
    {
    public:
        SymmetryFieldContext(antlr4::ParserRuleContext* parent, size_t invokingState);
        virtual size_t getRuleIndex() const override;
        std::vector<ElementListContext*> elementList();
        ElementListContext* elementList(size_t i);
        antlr4::tree::TerminalNode* EOF();

        virtual void enterRule(antlr4::tree::ParseTreeListener* listener) override;
        virtual void exitRule(antlr4::tree::ParseTreeListener* listener) override;

        virtual std::any accept(antlr4::tree::ParseTreeVisitor* visitor) override;
    };

    SymmetryFieldContext* symmetryField();

    class ElementListContext: public antlr4::ParserRuleContext
    {
    public:
        ElementListContext(antlr4::ParserRuleContext* parent, size_t invokingState);
        virtual size_t getRuleIndex() const override;
        std::vector<ElementContext*> element();
        ElementContext* element(size_t i);

        virtual void enterRule(antlr4::tree::ParseTreeListener* listener) override;
        virtual void exitRule(antlr4::tree::ParseTreeListener* listener) override;

        virtual std::any accept(antlr4::tree::ParseTreeVisitor* visitor) override;
    };

    ElementListContext* elementList();

    class ElementContext: public antlr4::ParserRuleContext
    {
    public:
        ElementContext(antlr4::ParserRuleContext* parent, size_t invokingState);
        virtual size_t getRuleIndex() const override;
        antlr4::tree::TerminalNode* ID();

        virtual void enterRule(antlr4::tree::ParseTreeListener* listener) override;
        virtual void exitRule(antlr4::tree::ParseTreeListener* listener) override;

        virtual std::any accept(antlr4::tree::ParseTreeVisitor* visitor) override;
    };

    ElementContext* element();

    // By default the static state used to implement the parser is lazily initialized during the
    // first call to the constructor. You can call this function if you wish to initialize the
    // static state ahead of time.
    static void initialize();

private:
};
