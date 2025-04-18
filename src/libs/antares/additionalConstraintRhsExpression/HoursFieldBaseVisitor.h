
// Generated from HoursField.g4 by ANTLR 4.13.2

#pragma once


#include "antlr4-runtime.h"
#include "HoursFieldVisitor.h"


/**
 * This class provides an empty implementation of HoursFieldVisitor, which can be
 * extended to create a visitor which only needs to handle a subset of the available methods.
 */
class  HoursFieldBaseVisitor : public HoursFieldVisitor {
public:
    virtual std::any visitHoursField(HoursFieldParser::HoursFieldContext* ctx) override
    {
        return visitChildren(ctx);
    }

    virtual std::any visitGroup(HoursFieldParser::GroupContext* ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitHour(HoursFieldParser::HourContext *ctx) override {
    return visitChildren(ctx);
  }


};

