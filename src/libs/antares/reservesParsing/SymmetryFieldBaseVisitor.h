
// Generated from SymmetryField.g4 by ANTLR 4.13.2

#pragma once


#include "antlr4-runtime.h"
#include "SymmetryFieldVisitor.h"


/**
 * This class provides an empty implementation of SymmetryFieldVisitor, which can be
 * extended to create a visitor which only needs to handle a subset of the available methods.
 */
class  SymmetryFieldBaseVisitor : public SymmetryFieldVisitor {
public:

  virtual std::any visitSymmetryField(SymmetryFieldParser::SymmetryFieldContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitElementList(SymmetryFieldParser::ElementListContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitElement(SymmetryFieldParser::ElementContext *ctx) override {
    return visitChildren(ctx);
  }


};

