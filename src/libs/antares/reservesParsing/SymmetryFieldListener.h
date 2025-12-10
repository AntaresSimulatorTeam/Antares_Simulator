
// Generated from SymmetryField.g4 by ANTLR 4.13.2

#pragma once


#include "antlr4-runtime.h"
#include "SymmetryFieldParser.h"


/**
 * This interface defines an abstract listener for a parse tree produced by SymmetryFieldParser.
 */
class  SymmetryFieldListener : public antlr4::tree::ParseTreeListener {
public:

  virtual void enterSymmetryField(SymmetryFieldParser::SymmetryFieldContext *ctx) = 0;
  virtual void exitSymmetryField(SymmetryFieldParser::SymmetryFieldContext *ctx) = 0;

  virtual void enterElementList(SymmetryFieldParser::ElementListContext *ctx) = 0;
  virtual void exitElementList(SymmetryFieldParser::ElementListContext *ctx) = 0;

  virtual void enterElement(SymmetryFieldParser::ElementContext *ctx) = 0;
  virtual void exitElement(SymmetryFieldParser::ElementContext *ctx) = 0;


};

