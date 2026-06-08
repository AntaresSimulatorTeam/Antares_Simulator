
// Generated from SymmetryField.g4 by ANTLR 4.13.2

#pragma once

#include "SymmetryFieldParser.h"
#include "antlr4-runtime.h"

/**
 * This class defines an abstract visitor for a parse tree
 * produced by SymmetryFieldParser.
 */
class SymmetryFieldVisitor: public antlr4::tree::AbstractParseTreeVisitor
{
public:
    /**
     * Visit parse trees produced by SymmetryFieldParser.
     */
    virtual std::any visitSymmetryField(SymmetryFieldParser::SymmetryFieldContext* context) = 0;

    virtual std::any visitElementList(SymmetryFieldParser::ElementListContext* context) = 0;

    virtual std::any visitElement(SymmetryFieldParser::ElementContext* context) = 0;
};
