
// Generated from Expr.g4 by ANTLR 4.13.2

#pragma once


#include "antlr4-runtime.h"
#include "ExprParser.h"



/**
 * This class defines an abstract visitor for a parse tree
 * produced by ExprParser.
 */
class  ExprVisitor : public antlr4::tree::AbstractParseTreeVisitor {
public:

  /**
   * Visit parse trees produced by ExprParser.
   */
    virtual std::any visitFullexpr(ExprParser::FullexprContext *context) = 0;

    virtual std::any visitTimeSum(ExprParser::TimeSumContext *context) = 0;

    virtual std::any visitNegation(ExprParser::NegationContext *context) = 0;

    virtual std::any visitUnsignedAtom(ExprParser::UnsignedAtomContext *context) = 0;

    virtual std::any visitExpression(ExprParser::ExpressionContext *context) = 0;

    virtual std::any visitTimeIndex(ExprParser::TimeIndexContext *context) = 0;

    virtual std::any visitComparison(ExprParser::ComparisonContext *context) = 0;

    virtual std::any visitAllTimeSum(ExprParser::AllTimeSumContext *context) = 0;

    virtual std::any visitTimeShift(ExprParser::TimeShiftContext *context) = 0;

    virtual std::any visitFunction(ExprParser::FunctionContext *context) = 0;

    virtual std::any visitAddsub(ExprParser::AddsubContext *context) = 0;

    virtual std::any visitPortField(ExprParser::PortFieldContext *context) = 0;

    virtual std::any visitMuldiv(ExprParser::MuldivContext *context) = 0;

    virtual std::any visitNumber(ExprParser::NumberContext *context) = 0;

    virtual std::any visitIdentifier(ExprParser::IdentifierContext *context) = 0;

    virtual std::any visitShift(ExprParser::ShiftContext *context) = 0;

    virtual std::any visitSignedAtom(ExprParser::SignedAtomContext *context) = 0;

    virtual std::any visitSignedExpression(ExprParser::SignedExpressionContext *context) = 0;

    virtual std::any visitShiftMuldiv(ExprParser::ShiftMuldivContext *context) = 0;

    virtual std::any visitShiftAddsub(ExprParser::ShiftAddsubContext *context) = 0;

    virtual std::any visitRightExpression(ExprParser::RightExpressionContext *context) = 0;

    virtual std::any visitRightMuldiv(ExprParser::RightMuldivContext *context) = 0;

    virtual std::any visitRightAtom(ExprParser::RightAtomContext *context) = 0;


};

