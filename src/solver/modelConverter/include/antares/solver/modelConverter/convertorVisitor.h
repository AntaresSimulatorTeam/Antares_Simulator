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

#pragma once

#include <ExprVisitor.h>

#include <antares/solver/expressions/Registry.hxx>
#include <antares/solver/expressions/nodes/ExpressionsNodes.h>
#include <antares/solver/libObjectModel/model.h>

namespace Antares::Solver::ModelConverter
{

Nodes::Node* convertExpressionToNode(
  const std::string& exprStr,
  Antares::Solver::Registry<Antares::Solver::Nodes::Node>& registry,
  const ObjectModel::Model& model);

// Visitor to convert nodes to Antares::Solver::Nodes
// TODO add reference to model to be able to resolve names as either parameters or variables
class ConvertorVisitor: public ExprVisitor
{
public:
    ConvertorVisitor(Antares::Solver::Registry<Antares::Solver::Nodes::Node>& registry,
                     const ObjectModel::Model& model);

    std::any visit(antlr4::tree::ParseTree* tree) override;

    std::any visitIdentifier(ExprParser::IdentifierContext* context) override;
    std::any visitMuldiv(ExprParser::MuldivContext* context) override;
    std::any visitFullexpr(ExprParser::FullexprContext* context) override;
    std::any visitShift(ExprParser::ShiftContext* context) override;
    std::any visitNegation(ExprParser::NegationContext* context) override;
    std::any visitExpression(ExprParser::ExpressionContext* context) override;
    std::any visitComparison(ExprParser::ComparisonContext* context) override;
    std::any visitAddsub(ExprParser::AddsubContext* context) override;
    std::any visitPortField(ExprParser::PortFieldContext* context) override;
    std::any visitNumber(ExprParser::NumberContext* context) override;
    std::any visitTimeIndex(ExprParser::TimeIndexContext* context) override;
    std::any visitTimeShift(ExprParser::TimeShiftContext* context) override;
    std::any visitFunction(ExprParser::FunctionContext* context) override;

    std::any visitTimeSum(ExprParser::TimeSumContext* context) override;
    std::any visitAllTimeSum(ExprParser::AllTimeSumContext* context) override;
    std::any visitSignedAtom(ExprParser::SignedAtomContext* context) override;
    std::any visitUnsignedAtom(ExprParser::UnsignedAtomContext* context) override;
    std::any visitRightAtom(ExprParser::RightAtomContext* context) override;
    std::any visitSignedExpression(ExprParser::SignedExpressionContext* context) override;
    std::any visitShiftAddsub(ExprParser::ShiftAddsubContext* context) override;
    std::any visitShiftMuldiv(ExprParser::ShiftMuldivContext* context) override;
    std::any visitRightMuldiv(ExprParser::RightMuldivContext* context) override;
    std::any visitRightExpression(ExprParser::RightExpressionContext* context) override;

    Antares::Solver::Registry<Antares::Solver::Nodes::Node>& registry_;
    const ObjectModel::Model& model_;
};

} // namespace Antares::Solver::ModelConverter
