
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

#include "ExprVisitor.h"
#include "antlr4-runtime.h"

/**
 * This class provides an empty implementation of ExprVisitor, which can be
 * extended to create a visitor which only needs to handle a subset of the available methods.
 */
class ExprBaseVisitor: public ExprVisitor
{
public:
    virtual std::any visitFullexpr(ExprParser::FullexprContext* ctx) override
    {
        return visitChildren(ctx);
    }

    virtual std::any visitShift(ExprParser::ShiftContext* ctx) override
    {
        return visitChildren(ctx);
    }

    virtual std::any visitIdentifier(ExprParser::IdentifierContext* ctx) override
    {
        return visitChildren(ctx);
    }

    virtual std::any visitNegation(ExprParser::NegationContext* ctx) override
    {
        return visitChildren(ctx);
    }

    virtual std::any visitExpression(ExprParser::ExpressionContext* ctx) override
    {
        return visitChildren(ctx);
    }

    virtual std::any visitComparison(ExprParser::ComparisonContext* ctx) override
    {
        return visitChildren(ctx);
    }

    virtual std::any visitAddsub(ExprParser::AddsubContext* ctx) override
    {
        return visitChildren(ctx);
    }

    virtual std::any visitPortField(ExprParser::PortFieldContext* ctx) override
    {
        return visitChildren(ctx);
    }

    virtual std::any visitMuldiv(ExprParser::MuldivContext* ctx) override
    {
        return visitChildren(ctx);
    }

    virtual std::any visitNumber(ExprParser::NumberContext* ctx) override
    {
        return visitChildren(ctx);
    }

    virtual std::any visitTimeIndex(ExprParser::TimeIndexContext* ctx) override
    {
        return visitChildren(ctx);
    }

    virtual std::any visitTimeShift(ExprParser::TimeShiftContext* ctx) override
    {
        return visitChildren(ctx);
    }

    virtual std::any visitFunction(ExprParser::FunctionContext* ctx) override
    {
        return visitChildren(ctx);
    }

    virtual std::any visitTimeShiftRange(ExprParser::TimeShiftRangeContext* ctx) override
    {
        return visitChildren(ctx);
    }

    virtual std::any visitTimeRange(ExprParser::TimeRangeContext* ctx) override
    {
        return visitChildren(ctx);
    }
};
