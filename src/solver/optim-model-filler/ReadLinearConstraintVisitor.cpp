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

#include <antares/solver/expressions/nodes/ExpressionsNodes.h>
#include <antares/solver/optim-model-filler/ReadLinearConstraintVisitor.h>
#include "antares/solver/expressions/nodes/EqualNode.h"

using namespace Antares::Optimization;

static std::map<std::string, double> add_maps(std::map<std::string, double> left,
                                              std::map<std::string, double> right,
                                              double rhs_multiplier)
{
    std::map result(left);
    for (auto [var_id, coef]: right)
    {
        if (result.contains(var_id))
        {
            result[var_id] += rhs_multiplier * coef;
        }
        else
        {
            result[var_id] = coef;
        }
    }
    return result;
}

static std::map<std::string, double> scale_map(std::map<std::string, double> map, double scale)
{
    std::map<std::string, double> result;
    for (auto [var_id, coef]: map)
    {
        result[var_id] = scale * coef;
    }
    return result;
}

// Subtract two maps<string, double>
static std::map<std::string, double> subtract(std::map<std::string, double> left,
                                              std::map<std::string, double> right)
{
    return add_maps(left, right, -1);
}

// Add two maps<string, double>
static std::map<std::string, double> add(std::map<std::string, double> left,
                                         std::map<std::string, double> right)
{
    return add_maps(left, right, 1);
}

static LinearConstraint add(LinearConstraint left, LinearConstraint right)
{
    return LinearConstraint{.scalar_value = left.scalar_value + right.scalar_value,
                            .coef_per_var = add(left.coef_per_var, right.coef_per_var)};
}

static LinearConstraint multiply(LinearConstraint left, LinearConstraint right)
{
    if (left.coef_per_var.empty())
    {
        return LinearConstraint{.scalar_value = left.scalar_value * right.scalar_value,
                                .coef_per_var = scale_map(right.coef_per_var, left.scalar_value)};
    }
    else
    {
        return LinearConstraint{.scalar_value = left.scalar_value * right.scalar_value,
                                .coef_per_var = scale_map(left.coef_per_var, right.scalar_value)};
    }
}

ReadLinearConstraintVisitor::ReadLinearConstraintVisitor(EvaluationContext context):
    context_(std::move(context))
{
}

std::string ReadLinearConstraintVisitor::name() const
{
    return "ReadLinearConstraintVisitor";
}

LinearConstraint ReadLinearConstraintVisitor::visit(const SumNode* sum_node)
{
    auto result = LinearConstraint();
    for (auto term_node: sum_node->getOperands())
    {
        result = add(result, dispatch(term_node));
    }
    return result;
}

LinearConstraint ReadLinearConstraintVisitor::visit(const SubtractionNode* node)
{
    return LinearConstraint();
}

LinearConstraint ReadLinearConstraintVisitor::visit(const MultiplicationNode* node)
{
    // do not allow (4 + 5*var1) * (6*var2 + 6) (not linear)
    auto left = dispatch(node->left());
    auto right = dispatch(node->right());
    if (!left.coef_per_var.empty() && !right.coef_per_var.empty())
    {
        // TODO : add test
        throw std::invalid_argument("Quadratic constraints are not implemented yet");
    }
    return multiply(left, right);
}

LinearConstraint ReadLinearConstraintVisitor::visit(const DivisionNode* node)
{
    return LinearConstraint();
}

LinearConstraint ReadLinearConstraintVisitor::visit(const EqualNode* node)
{
    // root node
    // TODO : interdire une 2e visite de ce type de node
    // var1 <= 2 * var1 +5
    // convention : ub=lb pour les neouds qui sont pas des comparateurs
    auto gauche = dispatch(node->left());  // scalaires, coeff de variables à "gauche"
    auto droite = dispatch(node->right()); // scalaires, coeff de variables à "droite"
    // tODO : retourner scalaires passés à droite, les coef passés à gauche
    return LinearConstraint{.scalar_value = -gauche.scalar_value + droite.scalar_value,
                            .coef_per_var = subtract(gauche.coef_per_var, droite.coef_per_var),
                            .sign = LinearConstraint::EQ};
}

LinearConstraint ReadLinearConstraintVisitor::visit(const LessThanOrEqualNode* node)
{
    // TODO : interdire une 2e visite de ce type de node
    auto gauche = dispatch(node->left());
    auto droite = dispatch(node->right());
    return LinearConstraint{.scalar_value = -gauche.scalar_value + droite.scalar_value,
                            .coef_per_var = subtract(gauche.coef_per_var, droite.coef_per_var),
                            .sign = LinearConstraint::LEQ};
}

LinearConstraint ReadLinearConstraintVisitor::visit(const GreaterThanOrEqualNode* node)
{
    // TODO : interdire une 2e visite de ce type de node
    auto gauche = dispatch(node->left());
    auto droite = dispatch(node->right());
    return LinearConstraint{.scalar_value = -gauche.scalar_value + droite.scalar_value,
                            .coef_per_var = subtract(gauche.coef_per_var, droite.coef_per_var),
                            .sign = LinearConstraint::GEQ};
}

LinearConstraint ReadLinearConstraintVisitor::visit(const NegationNode* node)
{
    return LinearConstraint();
}

LinearConstraint ReadLinearConstraintVisitor::visit(const VariableNode* node)
{
    return LinearConstraint{.scalar_value = 0, .coef_per_var = {{node->value(), 1}}};
}

LinearConstraint ReadLinearConstraintVisitor::visit(const ParameterNode* node)
{
    return LinearConstraint();
}

LinearConstraint ReadLinearConstraintVisitor::visit(const LiteralNode* node)
{
    return LinearConstraint{.scalar_value = node->value(), .coef_per_var = {}};
}

LinearConstraint ReadLinearConstraintVisitor::visit(const PortFieldNode* node)
{
    return LinearConstraint();
}

LinearConstraint ReadLinearConstraintVisitor::visit(const PortFieldSumNode* node)
{
    return LinearConstraint();
}

LinearConstraint ReadLinearConstraintVisitor::visit(const ComponentVariableNode* node)
{
    return LinearConstraint();
}

LinearConstraint ReadLinearConstraintVisitor::visit(const ComponentParameterNode* node)
{
    return LinearConstraint();
}
