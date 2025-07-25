
// Generated from HoursField.g4 by ANTLR 4.13.2

#pragma once


#include "antlr4-runtime.h"
#include "HoursFieldParser.h"



/**
 * This class defines an abstract visitor for a parse tree
 * produced by HoursFieldParser.
 */
class  HoursFieldVisitor : public antlr4::tree::AbstractParseTreeVisitor {
public:

  /**
   * Visit parse trees produced by HoursFieldParser.
   */
    virtual std::any visitHoursField(HoursFieldParser::HoursFieldContext *context) = 0;

    virtual std::any visitGroup(HoursFieldParser::GroupContext *context) = 0;

    virtual std::any visitHour(HoursFieldParser::HourContext *context) = 0;


};

