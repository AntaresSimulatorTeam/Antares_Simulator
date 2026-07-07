// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0
#pragma once
#include <SymmetryFieldBaseVisitor.h>

class SymmetryCollectorVisitor: public SymmetryFieldBaseVisitor
{
public:
    std::any visitSymmetryField(SymmetryFieldParser::SymmetryFieldContext* ctx) override
    {
        std::vector<std::set<std::string>> result;
        for (auto elementListCtx: ctx->elementList())
        {
            result.push_back(std::any_cast<std::set<std::string>>(visit(elementListCtx)));
        }
        return result;
    }

    std::any visitElementList(SymmetryFieldParser::ElementListContext* ctx) override
    {
        std::set<std::string> symmetry;
        for (auto elementCtx: ctx->element())
        {
            symmetry.insert(elementCtx->getText());
        }
        return symmetry;
    }
};
