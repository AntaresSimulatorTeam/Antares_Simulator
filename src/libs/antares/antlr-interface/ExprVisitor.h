
// Generated from Expr.g4 by ANTLR 4.7.2

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
    virtual antlrcpp::Any visitFullexpr(ExprParser::FullexprContext *context) = 0;

    virtual antlrcpp::Any visitTimeSum(ExprParser::TimeSumContext *context) = 0;

    virtual antlrcpp::Any visitNegation(ExprParser::NegationContext *context) = 0;

    virtual antlrcpp::Any visitUnsignedAtom(ExprParser::UnsignedAtomContext *context) = 0;

    virtual antlrcpp::Any visitExpression(ExprParser::ExpressionContext *context) = 0;

    virtual antlrcpp::Any visitTimeIndex(ExprParser::TimeIndexContext *context) = 0;

    virtual antlrcpp::Any visitComparison(ExprParser::ComparisonContext *context) = 0;

    virtual antlrcpp::Any visitAllTimeSum(ExprParser::AllTimeSumContext *context) = 0;

    virtual antlrcpp::Any visitTimeShift(ExprParser::TimeShiftContext *context) = 0;

    virtual antlrcpp::Any visitFunction(ExprParser::FunctionContext *context) = 0;

    virtual antlrcpp::Any visitAddsub(ExprParser::AddsubContext *context) = 0;

    virtual antlrcpp::Any visitPortField(ExprParser::PortFieldContext *context) = 0;

    virtual antlrcpp::Any visitMuldiv(ExprParser::MuldivContext *context) = 0;

    virtual antlrcpp::Any visitNumber(ExprParser::NumberContext *context) = 0;

    virtual antlrcpp::Any visitIdentifier(ExprParser::IdentifierContext *context) = 0;

    virtual antlrcpp::Any visitShift(ExprParser::ShiftContext *context) = 0;

    virtual antlrcpp::Any visitSignedAtom(ExprParser::SignedAtomContext *context) = 0;

    virtual antlrcpp::Any visitSignedExpression(ExprParser::SignedExpressionContext *context) = 0;

    virtual antlrcpp::Any visitShiftMuldiv(ExprParser::ShiftMuldivContext *context) = 0;

    virtual antlrcpp::Any visitShiftAddsub(ExprParser::ShiftAddsubContext *context) = 0;

    virtual antlrcpp::Any visitRightExpression(ExprParser::RightExpressionContext *context) = 0;

    virtual antlrcpp::Any visitRightMuldiv(ExprParser::RightMuldivContext *context) = 0;

    virtual antlrcpp::Any visitRightAtom(ExprParser::RightAtomContext *context) = 0;


};

