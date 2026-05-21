// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/adequacy-patch/gems-csr-adapter.h"

#include <stdexcept>
#include <string>

#include <antares/expressions/nodes/DivisionNode.h>
#include <antares/expressions/nodes/EqualNode.h>
#include <antares/expressions/nodes/GreaterThanOrEqualNode.h>
#include <antares/expressions/nodes/LessThanOrEqualNode.h>
#include <antares/expressions/nodes/LiteralNode.h>
#include <antares/expressions/nodes/MultiplicationNode.h>
#include <antares/expressions/nodes/NegationNode.h>
#include <antares/expressions/nodes/ParameterNode.h>
#include <antares/expressions/nodes/SubtractionNode.h>
#include <antares/expressions/nodes/SumNode.h>
#include <antares/expressions/nodes/VariableNode.h>
#include <antares/logs/logs.h>
#include <antares/study/system-model-base/variabilityType.h>

using namespace Antares;

namespace Antares::AdequacyPatch
{

GemsCsrAdapter::GemsCsrAdapter(const ModelerStudy::SystemModel::System& system,
                                const CsrProblemContext& csrCtx,
                                const std::regex& constraintFilter):
    system_(system),
    csrCtx_(csrCtx),
    constraintFilter_(constraintFilter)
{
    buildAreaVarMap();
}

void GemsCsrAdapter::buildAreaVarMap()
{
    if (!csrCtx_.areaToColumnIndex)
    {
        return;
    }

    for (const auto& component : system_.Components())
    {
        const auto* model = component.getModel();
        if (!model)
        {
            continue;
        }

        const auto& portToArea = component.portToAreaConnections();
        if (portToArea.empty())
        {
            continue;
        }

        const auto& pfDefs = model->PortFieldDefinitions();

        for (const auto& [portId, areaName] : portToArea)
        {
            if (csrCtx_.areaToColumnIndex->find(areaName) == csrCtx_.areaToColumnIndex->end())
            {
                logs.warning() << "[gems-csr-adapter] Area '" << areaName << "' (component '"
                               << component.Id() << "') not in CSR context, skipping";
                continue;
            }

            for (const auto& [key, pfd] : pfDefs)
            {
                if (key.portId != portId)
                {
                    continue;
                }
                const auto* root = pfd.Definition().RootNode();

                // Direct VariableNode: sign = +1
                if (const auto* varNode
                    = dynamic_cast<const Expressions::Nodes::VariableNode*>(root))
                {
                    VarKey vk{component.Id(), varNode->Index()};
                    pendingAreaFlows_.push_back({areaName, vk, +1.0});
                }
                // NegationNode(VariableNode): sign = -1
                else if (const auto* neg
                         = dynamic_cast<const Expressions::Nodes::NegationNode*>(root))
                {
                    if (const auto* varNode
                        = dynamic_cast<const Expressions::Nodes::VariableNode*>(neg->child()))
                    {
                        VarKey vk{component.Id(), varNode->Index()};
                        pendingAreaFlows_.push_back({areaName, vk, -1.0});
                    }
                }
            }
        }
    }
    csrCtx_.areaToColumnIndex = nullptr;
}

void GemsCsrAdapter::buildAreaFlowMap()
{
    for (const auto& pending : pendingAreaFlows_)
    {
        auto it = varIdToColIdx_.find(pending.varKey);
        if (it != varIdToColIdx_.end())
        {
            areaFlowContribs_.push_back({pending.areaName, it->second, pending.sign});
            logs.debug() << "[gems-csr-adapter] Area '" << pending.areaName
                         << "' GEMS exchange → CSR col " << it->second
                         << " (coeff " << pending.sign << ")";
        }
    }
}

void GemsCsrAdapter::registerExtraVariables(CsrProblemBuilder& builder)
{
    int count = 0;
    for (const auto& component : system_.Components())
    {
        const auto* model = component.getModel();
        if (!model)
        {
            continue;
        }

        bool hasMatchingConstraint = false;
        for (const auto& constraint : model->Constraints())
        {
            if (std::regex_search(constraint.Id(), constraintFilter_))
            {
                hasMatchingConstraint = true;
                break;
            }
        }
        if (!hasMatchingConstraint)
        {
            continue;
        }

        const auto& vars = model->Variables();
        for (unsigned int idx = 0; idx < vars.size(); ++idx)
        {
            VarKey vk{component.Id(), idx};
            if (varIdToColIdx_.count(vk))
            {
                continue;
            }
            const std::string colName = component.Id() + "." + vars[idx].Id();
            static constexpr double kInf = 1e20;
            int col = builder.allocateColumn(colName, -kInf, kInf);
            varIdToColIdx_[vk] = col;
            ++count;
            logs.debug() << "[gems-csr-adapter] Allocated CSR column " << col
                         << " for variable '" << colName << "'";
        }
    }
    // Only record the count on the first registration; re-entries allocate 0 new
    // columns (all are already in varIdToColIdx_) and must not clobber the count.
    if (!extraVarsRegistered_)
    {
        extraVarCount_ = count;
        buildAreaFlowMap();
    }
    extraVarsRegistered_ = true;
}

double GemsCsrAdapter::resolveParameter(const ModelerStudy::SystemModel::Component& component,
                                        const std::string& paramName,
                                        unsigned int hour,
                                        unsigned int tsNumber) const
{
    const auto& paramMap = component.getParameterValues();
    const auto it = paramMap.find(paramName);
    if (it == paramMap.end())
    {
        throw std::runtime_error("Parameter '" + paramName + "' not found in component '"
                                 + component.Id() + "'");
    }
    const auto& ptv = it->second;

    switch (ptv.type)
    {
    case Optimisation::VariabilityType::CONSTANT_IN_TIME_AND_SCENARIO:
        return std::stod(ptv.value);
    case Optimisation::VariabilityType::VARYING_IN_TIME_ONLY:
        return csrCtx_.dataSeries->getData(ptv.value, 0, hour);
    case Optimisation::VariabilityType::VARYING_IN_SCENARIO_ONLY:
        return csrCtx_.dataSeries->getData(ptv.value, tsNumber, 0);
    case Optimisation::VariabilityType::VARYING_IN_TIME_AND_SCENARIO:
        return csrCtx_.dataSeries->getData(ptv.value, tsNumber, hour);
    }
    throw std::runtime_error("Unknown variability type for parameter '" + paramName + "'");
}

GemsCsrAdapter::LinearExpr GemsCsrAdapter::evalExpr(
  const Expressions::Nodes::Node* node,
  const ModelerStudy::SystemModel::Component& component,
  unsigned int hour,
  unsigned int tsNumber) const
{
    using namespace Expressions::Nodes;

    if (const auto* lit = dynamic_cast<const LiteralNode*>(node))
    {
        return LinearExpr{{}, lit->value()};
    }

    if (const auto* param = dynamic_cast<const ParameterNode*>(node))
    {
        double val = resolveParameter(component, param->value(), hour, tsNumber);
        return LinearExpr{{}, val};
    }

    if (const auto* var = dynamic_cast<const VariableNode*>(node))
    {
        VarKey vk{component.Id(), var->Index()};
        auto it = varIdToColIdx_.find(vk);
        if (it == varIdToColIdx_.end())
        {
            throw std::runtime_error("Variable '" + var->value()
                                     + "' index=" + std::to_string(var->Index())
                                     + " not in column map for component '" + component.Id()
                                     + "'. Call registerExtraVariables() first.");
        }
        return LinearExpr{{{it->second, 1.0}}, 0.0};
    }

    if (const auto* neg = dynamic_cast<const NegationNode*>(node))
    {
        auto child = evalExpr(neg->child(), component, hour, tsNumber);
        for (auto& [col, coeff] : child.colTerms)
        {
            coeff = -coeff;
        }
        child.constant = -child.constant;
        return child;
    }

    if (const auto* mul = dynamic_cast<const MultiplicationNode*>(node))
    {
        auto left = evalExpr(mul->left(), component, hour, tsNumber);
        auto right = evalExpr(mul->right(), component, hour, tsNumber);
        if (!left.colTerms.empty() && !right.colTerms.empty())
        {
            throw std::runtime_error("Non-linear multiplication in constraint for component '"
                                     + component.Id() + "'");
        }
        if (left.colTerms.empty())
        {
            double factor = left.constant;
            for (auto& [col, coeff] : right.colTerms)
            {
                coeff *= factor;
            }
            right.constant *= factor;
            return right;
        }
        double factor = right.constant;
        for (auto& [col, coeff] : left.colTerms)
        {
            coeff *= factor;
        }
        left.constant *= factor;
        return left;
    }

    if (const auto* div = dynamic_cast<const DivisionNode*>(node))
    {
        auto left = evalExpr(div->left(), component, hour, tsNumber);
        auto right = evalExpr(div->right(), component, hour, tsNumber);
        if (!right.colTerms.empty())
        {
            throw std::runtime_error("Division by variable in constraint for component '"
                                     + component.Id() + "'");
        }
        if (right.constant == 0.0)
        {
            throw std::runtime_error("Division by zero in constraint for component '"
                                     + component.Id() + "'");
        }
        double factor = 1.0 / right.constant;
        for (auto& [col, coeff] : left.colTerms)
        {
            coeff *= factor;
        }
        left.constant *= factor;
        return left;
    }

    if (const auto* sub = dynamic_cast<const SubtractionNode*>(node))
    {
        auto left = evalExpr(sub->left(), component, hour, tsNumber);
        auto right = evalExpr(sub->right(), component, hour, tsNumber);
        for (const auto& [col, coeff] : right.colTerms)
        {
            left.colTerms[col] -= coeff;
        }
        left.constant -= right.constant;
        return left;
    }

    if (const auto* sum = dynamic_cast<const SumNode*>(node))
    {
        LinearExpr result;
        for (const auto* operand : sum->getOperands())
        {
            auto child = evalExpr(operand, component, hour, tsNumber);
            for (const auto& [col, coeff] : child.colTerms)
            {
                result.colTerms[col] += coeff;
            }
            result.constant += child.constant;
        }
        return result;
    }

    throw std::runtime_error("Unsupported node type '" + node->name()
                             + "' in constraint for component '" + component.Id() + "'");
}

bool GemsCsrAdapter::buildRow(const Expressions::Nodes::Node* root,
                              const ModelerStudy::SystemModel::Component& component,
                              CsrRow& row,
                              unsigned int hour,
                              unsigned int tsNumber) const
{
    using namespace Expressions::Nodes;

    CsrRowSense sense;
    const Node* lhsNode = nullptr;
    const Node* rhsNode = nullptr;

    if (const auto* n = dynamic_cast<const LessThanOrEqualNode*>(root))
    {
        sense = CsrRowSense::LE;
        lhsNode = n->left();
        rhsNode = n->right();
    }
    else if (const auto* n = dynamic_cast<const GreaterThanOrEqualNode*>(root))
    {
        sense = CsrRowSense::GE;
        lhsNode = n->left();
        rhsNode = n->right();
    }
    else if (const auto* n = dynamic_cast<const EqualNode*>(root))
    {
        sense = CsrRowSense::EQ;
        lhsNode = n->left();
        rhsNode = n->right();
    }
    else
    {
        logs.warning() << "[gems-csr-adapter] Constraint '" << row.constraintId
                       << "' root is not a comparison node (" << root->name() << "), skipping";
        return false;
    }

    try
    {
        auto lhs = evalExpr(lhsNode, component, hour, tsNumber);
        auto rhs = evalExpr(rhsNode, component, hour, tsNumber);

        // Standard form: (lhs_terms - rhs_terms) sense (rhs_constant - lhs_constant)
        for (const auto& [col, coeff] : rhs.colTerms)
        {
            lhs.colTerms[col] -= coeff;
        }
        row.sense = sense;
        row.rhs = rhs.constant - lhs.constant;
        for (const auto& [col, coeff] : lhs.colTerms)
        {
            if (coeff != 0.0)
            {
                row.terms.push_back({col, coeff});
            }
        }
    }
    catch (const std::exception& e)
    {
        logs.warning() << "[gems-csr-adapter] Failed to evaluate constraint '"
                       << row.constraintId << "': " << e.what();
        return false;
    }
    return true;
}

int GemsCsrAdapter::countExtraVariables() const
{
    // After registration the map is fully populated, so the loop below would
    // find nothing.  Return the count captured during registerExtraVariables().
    if (extraVarsRegistered_)
    {
        return extraVarCount_;
    }
    int count = 0;
    for (const auto& component : system_.Components())
    {
        const auto* model = component.getModel();
        if (!model)
        {
            continue;
        }
        bool hasMatch = false;
        for (const auto& constraint : model->Constraints())
        {
            if (std::regex_search(constraint.Id(), constraintFilter_))
            {
                hasMatch = true;
                break;
            }
        }
        if (!hasMatch)
        {
            continue;
        }
        const auto& vars = model->Variables();
        for (unsigned int idx = 0; idx < vars.size(); ++idx)
        {
            VarKey vk{component.Id(), idx};
            if (!varIdToColIdx_.count(vk))
            {
                ++count;
            }
        }
    }
    return count;
}

int GemsCsrAdapter::countMatchingConstraints() const
{
    int count = 0;
    for (const auto& component : system_.Components())
    {
        const auto* model = component.getModel();
        if (!model)
        {
            continue;
        }
        for (const auto& constraint : model->Constraints())
        {
            if (std::regex_search(constraint.Id(), constraintFilter_))
            {
                ++count;
            }
        }
    }
    return count;
}

std::vector<CsrRow> GemsCsrAdapter::rowsForHour(int hour, int mcYear) const
{
    if (!extraVarsRegistered_)
    {
        logs.warning() << "[gems-csr-adapter] rowsForHour() called before "
                          "registerExtraVariables(); non-area-mapped variables may be missing";
    }

    std::vector<CsrRow> rows;
    const unsigned int uHour = static_cast<unsigned int>(hour);
    // EmptyScenario equivalent: TS number = 1 (single scenario, no wiring yet)
    const unsigned int tsNumber = static_cast<unsigned int>(mcYear + 1);

    for (const auto& component : system_.Components())
    {
        const auto* model = component.getModel();
        if (!model)
        {
            continue;
        }

        for (const auto& constraint : model->Constraints())
        {
            if (!std::regex_search(constraint.Id(), constraintFilter_))
            {
                continue;
            }

            const auto* root = constraint.expression().RootNode();
            if (!root)
            {
                logs.warning() << "[gems-csr-adapter] Constraint '" << constraint.Id()
                               << "' (component '" << component.Id()
                               << "') has empty expression, skipping";
                continue;
            }

            CsrRow row;
            row.constraintId = component.Id() + "." + constraint.Id();

            if (buildRow(root, component, row, uHour, tsNumber))
            {
                rows.push_back(std::move(row));
            }
        }
    }
    return rows;
}

} // namespace Antares::AdequacyPatch