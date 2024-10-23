
// Generated from Expr.g4 by ANTLR 4.7.2

#pragma once


#include "antlr4-runtime.h"
#include "ExprVisitor.h"


/**
 * This class provides an empty implementation of ExprVisitor, which can be
 * extended to create a visitor which only needs to handle a subset of the available methods.
 */
class  ExprBaseVisitor : public ExprVisitor {
public:

  virtual antlrcpp::Any visitFullexpr(ExprParser::FullexprContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitTimeSum(ExprParser::TimeSumContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitNegation(ExprParser::NegationContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitUnsignedAtom(ExprParser::UnsignedAtomContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitExpression(ExprParser::ExpressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitTimeIndex(ExprParser::TimeIndexContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitComparison(ExprParser::ComparisonContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitAllTimeSum(ExprParser::AllTimeSumContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitTimeShift(ExprParser::TimeShiftContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitFunction(ExprParser::FunctionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitAddsub(ExprParser::AddsubContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitPortField(ExprParser::PortFieldContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitMuldiv(ExprParser::MuldivContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitNumber(ExprParser::NumberContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitIdentifier(ExprParser::IdentifierContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitShift(ExprParser::ShiftContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitSignedAtom(ExprParser::SignedAtomContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitSignedExpression(ExprParser::SignedExpressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitShiftMuldiv(ExprParser::ShiftMuldivContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitShiftAddsub(ExprParser::ShiftAddsubContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitRightExpression(ExprParser::RightExpressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitRightMuldiv(ExprParser::RightMuldivContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitRightAtom(ExprParser::RightAtomContext *ctx) override {
    return visitChildren(ctx);
  }


};

