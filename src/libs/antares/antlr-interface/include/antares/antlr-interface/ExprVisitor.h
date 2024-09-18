
/*
 * Copyright 2007-2024, RTE (https://www.rte-france.com)
 * See AUTHORS.txt
 * SPDX-License-Identifier: MPL-2.0
 * This file is part of Antares-Simulator,
 * Adequacy and Performance assessment for interconnected energy networks.
 *
 * Antares_Simulator is free software: you can redistribute it and/or modify
 * it under the terms of the Mozilla Public Licence 2.0 as published by
 * the Mozilla Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * Antares_Simulator is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * Mozilla Public Licence 2.0 for more details.
 *
 * You should have received a copy of the Mozilla Public Licence 2.0
 * along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
 */

// Generated from Expr.g4 by ANTLR 4.13.1

#pragma once

#include "ExprParser.h"
#include "antlr4-runtime.h"

/**
 * This class defines an abstract visitor for a parse tree
 * produced by ExprParser.
 */
class ExprVisitor: public antlr4::tree::AbstractParseTreeVisitor
{
public:
    /**
     * Visit parse trees produced by ExprParser.
     */
    virtual std::any visitFullexpr(ExprParser::FullexprContext* context) = 0;

    virtual std::any visitShift(ExprParser::ShiftContext* context) = 0;

    virtual std::any visitIdentifier(ExprParser::IdentifierContext* context) = 0;

    virtual std::any visitNegation(ExprParser::NegationContext* context) = 0;

    virtual std::any visitExpression(ExprParser::ExpressionContext* context) = 0;

    virtual std::any visitComparison(ExprParser::ComparisonContext* context) = 0;

    virtual std::any visitAddsub(ExprParser::AddsubContext* context) = 0;

    virtual std::any visitPortField(ExprParser::PortFieldContext* context) = 0;

    virtual std::any visitMuldiv(ExprParser::MuldivContext* context) = 0;

    virtual std::any visitNumber(ExprParser::NumberContext* context) = 0;

    virtual std::any visitTimeIndex(ExprParser::TimeIndexContext* context) = 0;

    virtual std::any visitTimeShift(ExprParser::TimeShiftContext* context) = 0;

    virtual std::any visitFunction(ExprParser::FunctionContext* context) = 0;

    virtual std::any visitTimeShiftRange(ExprParser::TimeShiftRangeContext* context) = 0;

    virtual std::any visitTimeRange(ExprParser::TimeRangeContext* context) = 0;
};
