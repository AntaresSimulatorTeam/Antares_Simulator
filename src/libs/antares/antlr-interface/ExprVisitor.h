
// Generated from Expr.g4 by ANTLR 4.13.1

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

    virtual std::any visitShift(ExprParser::ShiftContext *context) = 0;

    virtual std::any visitIdentifier(ExprParser::IdentifierContext *context) = 0;

    virtual std::any visitNegation(ExprParser::NegationContext *context) = 0;

    virtual std::any visitExpression(ExprParser::ExpressionContext *context) = 0;

    virtual std::any visitComparison(ExprParser::ComparisonContext *context) = 0;

    virtual std::any visitAddsub(ExprParser::AddsubContext *context) = 0;

    virtual std::any visitPortField(ExprParser::PortFieldContext *context) = 0;

    virtual std::any visitMuldiv(ExprParser::MuldivContext *context) = 0;

    virtual std::any visitNumber(ExprParser::NumberContext *context) = 0;

    virtual std::any visitTimeIndex(ExprParser::TimeIndexContext *context) = 0;

    virtual std::any visitTimeShift(ExprParser::TimeShiftContext *context) = 0;

    virtual std::any visitFunction(ExprParser::FunctionContext *context) = 0;

    virtual std::any visitTimeShiftRange(ExprParser::TimeShiftRangeContext *context) = 0;

    virtual std::any visitTimeRange(ExprParser::TimeRangeContext *context) = 0;


};

