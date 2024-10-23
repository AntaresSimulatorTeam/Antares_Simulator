
// Generated from Expr.g4 by ANTLR 4.13.2

#pragma once


#include "antlr4-runtime.h"
#include "ExprVisitor.h"


/**
 * This class provides an empty implementation of ExprVisitor, which can be
 * extended to create a visitor which only needs to handle a subset of the available methods.
 */
class  ExprBaseVisitor : public ExprVisitor {
public:

  virtual std::any visitFullexpr(ExprParser::FullexprContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitTimeSum(ExprParser::TimeSumContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitNegation(ExprParser::NegationContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitUnsignedAtom(ExprParser::UnsignedAtomContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitExpression(ExprParser::ExpressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitTimeIndex(ExprParser::TimeIndexContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitComparison(ExprParser::ComparisonContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitAllTimeSum(ExprParser::AllTimeSumContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitTimeShift(ExprParser::TimeShiftContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitFunction(ExprParser::FunctionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitAddsub(ExprParser::AddsubContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitPortField(ExprParser::PortFieldContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitMuldiv(ExprParser::MuldivContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitNumber(ExprParser::NumberContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitIdentifier(ExprParser::IdentifierContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitShift(ExprParser::ShiftContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitSignedAtom(ExprParser::SignedAtomContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitSignedExpression(ExprParser::SignedExpressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitShiftMuldiv(ExprParser::ShiftMuldivContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitShiftAddsub(ExprParser::ShiftAddsubContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitRightExpression(ExprParser::RightExpressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitRightMuldiv(ExprParser::RightMuldivContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitRightAtom(ExprParser::RightAtomContext *ctx) override {
    return visitChildren(ctx);
  }


};

