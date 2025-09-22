/*
 * Copyright 2007-2025, RTE (https://www.rte-france.com)
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

#include <numeric>
#include <ranges>
#include <stdexcept>
#include <variant>

#include <antares/expressions/nodes/ExpressionsNodes.h>
#include <antares/expressions/visitors/EvalVisitor.h>
#include <antares/solver/optim-model-filler/ComponentFiller.h>
#include <antares/study/system-model/variable.h>
#include "antares/expressions/visitors/TimeIndexVisitor.h"
#include "antares/modeler-optimisation-container/scenarioGroupRepo.h"

namespace
{
template<typename T>
std::optional<T> buildOptional(bool condition, T value)
{
    if (condition)
    {
        return value;
    }
    else
    {
        return {};
    }
}

template<class T>
void rotate(T& v, int shift)
{
    if (!v.empty())
    {
        return;
    }
    const int n = static_cast<int>(v.size());
    const int k = ((shift % n) + n) % n;
    std::rotate(v.begin(), v.begin() + k, v.end());
}
} // namespace

namespace V
{
struct SingleTimeExpr final
{
    std::vector<std::pair<int, double>> coefs;
    double constant = 0.;

    SingleTimeExpr() = default;

    SingleTimeExpr(const std::vector<std::pair<int, double>>& coefs, double constant):
        coefs(coefs),
        constant(constant)
    {
    }

    SingleTimeExpr& operator+=(const SingleTimeExpr& other)
    {
        coefs.insert(coefs.end(), other.coefs.begin(), other.coefs.end());
        constant += other.constant;
        return *this;
    }

    SingleTimeExpr& operator*=(const SingleTimeExpr& other)
    {
        if (hasCoefs() && other.hasCoefs())
        {
            // Multiplying two symbolic expressions would give quadratic terms,
            // which this representation cannot hold.
            throw std::runtime_error("Quadratic term detected");
        }
        else if (!hasCoefs() && !other.hasCoefs())
        {
            // constant * constant
            constant *= other.constant;
        }
        else if (hasCoefs() && !other.hasCoefs())
        {
            // linear * constant
            for (auto& [idx, coef]: coefs)
            {
                coef *= other.constant;
            }
            constant *= other.constant;
        }
        else // (!hasCoefs() && other.hasCoefs())
        {
            // constant * linear
            coefs = other.coefs;
            for (auto& [idx, coef]: coefs)
            {
                coef *= constant; // use this->constant as multiplier
            }
            constant *= other.constant;
        }
        return *this;
    }

    bool hasCoefs() const
    {
        return !coefs.empty();
    }
};

class AllTimeExpr final
{
public:
    AllTimeExpr(std::size_t nbTimesteps)
    {
        if (nbTimesteps == 1)
        {
            v_.emplace<0>();
        }
        else
        {
            v_.emplace<1>(nbTimesteps);
        }
    }

    AllTimeExpr(SingleTimeExpr&& expr):
        v_(std::move(expr))
    {
    }

    void expandTo(std::size_t nbTimesteps)
    {
        if (auto* expr = std::get_if<SingleTimeExpr>(&v_))
        {
            v_.emplace<1>(nbTimesteps, *expr);
        }
    }

    auto begin()
    {
        if (auto* expr = std::get_if<SingleTimeExpr>(&v_))
        {
            return expr;
        }
        if (auto* expr = std::get_if<std::vector<SingleTimeExpr>>(&v_))
        {
            return expr->data();
        }
        throw std::runtime_error("Invalid variant");
    }

    auto begin() const
    {
        if (const auto* expr = std::get_if<SingleTimeExpr>(&v_))
        {
            return expr;
        }
        if (const auto* expr = std::get_if<std::vector<SingleTimeExpr>>(&v_))
        {
            return expr->data();
        }
        throw std::runtime_error("Invalid variant");
    }

    auto end()
    {
        if (auto* expr = std::get_if<SingleTimeExpr>(&v_))
        {
            return expr + 1;
        }
        if (auto* expr = std::get_if<std::vector<SingleTimeExpr>>(&v_))
        {
            return expr->data() + expr->size();
        }
        throw std::runtime_error("Invalid variant");
    }

    auto end() const
    {
        if (const auto* expr = std::get_if<SingleTimeExpr>(&v_))
        {
            return expr + 1;
        }
        if (const auto* expr = std::get_if<std::vector<SingleTimeExpr>>(&v_))
        {
            return expr->data() + expr->size();
        }
        throw std::runtime_error("Invalid variant");
    }

    SingleTimeExpr& operator[](std::size_t idx)
    {
        if (auto* expr = std::get_if<SingleTimeExpr>(&v_))
        {
            return *expr;
        }
        if (auto* expr = std::get_if<std::vector<SingleTimeExpr>>(&v_))
        {
            return expr->operator[](idx);
        }
        throw std::runtime_error("Invalid variant");
    }

    const SingleTimeExpr& operator[](std::size_t idx) const
    {
        if (const auto* expr = std::get_if<SingleTimeExpr>(&v_))
        {
            return *expr;
        }
        if (const auto* expr = std::get_if<std::vector<SingleTimeExpr>>(&v_))
        {
            return expr->operator[](idx);
        }
        throw std::runtime_error("Invalid variant");
    }

    std::size_t size() const
    {
        if (const auto* expr = std::get_if<SingleTimeExpr>(&v_))
        {
            return 1;
        }
        if (const auto* expr = std::get_if<std::vector<SingleTimeExpr>>(&v_))
        {
            return expr->size();
        }
        throw std::runtime_error("Invalid variant");
    }

    AllTimeExpr& operator+=(const AllTimeExpr& other)
    {
        if (other.size() > size())
        {
            expandTo(other.size());
        }
        for (std::size_t t = 0; t < size(); ++t)
        {
            this->operator[](t) += other[t];
        }
        return *this;
    }

    AllTimeExpr& operator*=(const AllTimeExpr& other)
    {
        if (other.size() > size())
        {
            expandTo(other.size());
        }
        int t = 0;
        for (auto& expr: *this)
        {
            expr *= other[t];
            t++;
        }
        return *this;
    }

    AllTimeExpr operator-() const
    {
        AllTimeExpr result = *this;
        for (auto& expr: result)
        {
            for (auto& [idx, coef]: expr.coefs)
            {
                coef = -coef;
            }
            expr.constant = -expr.constant;
        }
        return result;
    }

    SingleTimeExpr* asSingle()
    {
        return std::get_if<SingleTimeExpr>(&v_);
    }

    std::vector<SingleTimeExpr>* asMultiple()
    {
        return std::get_if<std::vector<SingleTimeExpr>>(&v_);
    }

private:
    std::variant<SingleTimeExpr, std::vector<SingleTimeExpr>> v_;
};

using namespace Antares::Expressions;
using namespace Antares::Expressions::Nodes;

class Visitor: public Visitors::NodeVisitor<AllTimeExpr>
{
public:
    /**
     * @brief Constructs a clone visitor with the specified registry for creating new nodes.
     *
     * @param registry The registry used for creating new nodes.
     */
    explicit Visitor(const Antares::Optimisation::OptimEntityContainer& optimEntityContainer,
                     const Antares::ModelerStudy::SystemModel::Component& component,
                     const Antares::Optimisation::LinearProblemApi::FillContext& fillContext,
                     int nbtimeSteps):
        optimEntityContainer_(optimEntityContainer),
        component_(component),
        nbtimeSteps_(nbtimeSteps),
        fillContext_(fillContext),
        evalContext_(optimEntityContainer.getOptimComponent(component.Index()).evaluationContext),
        evalVisitor_(optimEntityContainer, fillContext, component)
    {
    }

    std::string name() const override
    {
        return "V::Visitor";
    }

    AllTimeExpr visit(const Nodes::SumNode* node) override
    {
        const auto& operands = node->getOperands();
        AllTimeExpr ret(nbtimeSteps_);
        for (auto* operand: operands)
        {
            ret += dispatch(operand);
        }
        return ret;
    }

    AllTimeExpr visit(const Nodes::SubtractionNode* node) override
    {
        throw std::runtime_error("Not implemented");
    }

    AllTimeExpr visit(const Nodes::MultiplicationNode* node) override
    {
        auto ret = dispatch(node->left());
        ret *= dispatch(node->right());
        return ret;
    }

    AllTimeExpr visit(const Nodes::DivisionNode* node) override
    {
        throw std::runtime_error("Not implemented");
    }

    AllTimeExpr visit(const Nodes::EqualNode* node) override
    {
        throw std::runtime_error("Not implemented");
    }

    AllTimeExpr visit(const Nodes::LessThanOrEqualNode* node) override
    {
        throw std::runtime_error("Not implemented");
    }

    AllTimeExpr visit(const Nodes::GreaterThanOrEqualNode* node) override
    {
        throw std::runtime_error("Not implemented");
    }

    AllTimeExpr visit(const Nodes::NegationNode* node) override
    {
        throw std::runtime_error("Not implemented");
    }

    AllTimeExpr visit(const Nodes::VariableNode* node) override
    {
        const auto& optimComponent = optimEntityContainer_.getOptimComponent(component_.Index());
        const auto globalIndex = optimComponent.variableIndexMap.at(
          node->value()); // the only time we search in a map
        const auto& variableStartColumn = optimEntityContainer_.getVariableStartColumn();
        const auto variableStart = variableStartColumn.at(globalIndex);
        const auto variableEnd = variableStart == *variableStartColumn.rbegin()
                                   ? optimEntityContainer_.variablesSize()
                                   : variableStartColumn.at(globalIndex + 1);

        if (node->timeIndex() == Antares::Optimisation::TimeIndex::CONSTANT_IN_TIME_AND_SCENARIO)
        {
            SingleTimeExpr out;
            out.coefs.emplace_back(variableStart, 1);
            return AllTimeExpr(std::move(out));
        }
        // else time-dep only hanled    //  check if var is time-dep then nbTimeStep == variableEnd
        // - variableStart+1
        if (node->timeIndex() == Antares::Optimisation::TimeIndex::VARYING_IN_TIME_ONLY
            || node->timeIndex()
                 == Antares::Optimisation::TimeIndex::VARYING_IN_TIME_AND_SCENARIO) /* scenario not
                                                                                  handled !*/
        {
            AllTimeExpr out(nbtimeSteps_);

            auto variableIndex = variableStart;
            for (int ts = 0; ts < nbtimeSteps_; ts++)
            {
                out[ts].coefs.emplace_back(variableIndex, 1);
                ++variableIndex;
            }
            return out;
        }
        throw "the support of scenario dependent variables is not available for now";
    }

    AllTimeExpr visit(const Nodes::ParameterNode* node) override
    {
        throw std::runtime_error("Not implemented");
    }

    AllTimeExpr visit(const Nodes::LiteralNode* node) override
    {
        SingleTimeExpr ret({}, node->value()); // Constant expr
        return AllTimeExpr(std::move(ret));
    }

    AllTimeExpr visit(const Nodes::PortFieldNode* node) override
    {
        throw std::runtime_error("Not implemented");
    }

    AllTimeExpr visit(const Nodes::PortFieldSumNode* node) override
    {
        auto& portId = node->getPortName();
        auto& fieldId = node->getFieldName();

        AllTimeExpr to_return(nbtimeSteps_);

        for (const auto connexion_end: component_.componentConnectionsViaPort(portId))
        {
            auto* component = connexion_end.component();
            auto* port = connexion_end.port();

            Visitor visitor(optimEntityContainer_, *component, fillContext_, nbtimeSteps_);

            const Node* node = component->nodeAtPortField(port->Id(), fieldId);
            to_return += visitor.dispatch(node);
        }

        return to_return;
    }

    AllTimeExpr visit(const Nodes::TimeShiftNode* node) override
    {
        auto expression = dispatch(node->left());
        if (expression.size() == 1)
        {
            return expression;
        }
        // it must be single value:  expression[IHaveTobeEvaluatedAsSingleValue]
        const auto timeIndex = static_cast<int>(
          evalVisitor_.dispatch(node->right()).valueAsDouble());
        rotate(*expression.asMultiple(), timeIndex);
        return expression;
    }

    AllTimeExpr visit(const Nodes::TimeIndexNode* node) override
    {
        auto expression = dispatch(node->left());

        if (expression.size() == 1)
        {
            return expression;
        }
        // it must be single value:  expression[IHaveTobeEvaluatedAsSingleValue]
        const auto timeIndex = static_cast<int>(
          evalVisitor_.dispatch(node->right()).valueAsDouble());
        return AllTimeExpr(std::move(*(expression.begin() + timeIndex)));
    }

    AllTimeExpr visit(const Nodes::TimeSumNode* node) override
    {
        throw std::runtime_error("Not implemented");
    }

    AllTimeExpr visit(const Nodes::AllTimeSumNode* node) override
    {
        SingleTimeExpr ret; // Constant expr
        auto expr = dispatch(node->child());
        for (auto& s: expr)
        {
            ret += s;
        }
        return AllTimeExpr(std::move(ret));
    }

private:
    const Antares::Optimisation::OptimEntityContainer& optimEntityContainer_;
    const Antares::ModelerStudy::SystemModel::Component& component_;
    const Antares::Optimisation::EvaluationContext& evalContext_;
    const Antares::Optimisation::LinearProblemApi::FillContext& fillContext_;
    Antares::Expressions::Visitors::EvalVisitor evalVisitor_;
    int nbtimeSteps_;
};

} // namespace V

namespace Antares::Optimisation
{
VariablesBulkAddition::VariablesBulkAddition(
  Optimisation::LinearProblemApi::ILinearProblem& linear_problem,
  OptimEntityContainer& optimEntityContainer):
    linear_problem_(linear_problem),
    optimEntityContainer_(optimEntityContainer)
{
}

void VariablesBulkAddition::addVariable(const std::string& compoId,
                                        const std::string& variableId,
                                        double lb,
                                        double ub,
                                        bool integer,
                                        const Optimization::Dimensions& dim) const
{
    optimEntityContainer_.addStartColumn();
    for (const auto& s: dim.getScenarioIndices())
    {
        for (const auto t: dim.getTimesteps())
        {
            auto year = buildOptional<Optimization::MCYearAndTime::MCYear>(
              dim.isScenarioDependent(),
              static_cast<Optimization::MCYearAndTime::MCYear>(s));
            const auto ts = buildOptional(dim.isTimeDependent(), t);
            optimEntityContainer_.registerVariable(
              linear_problem_.addVariable(lb,
                                          ub,
                                          integer,
                                          buildVariableName(compoId, variableId, year, ts)));
        }
    }
}

void VariablesBulkAddition::addVariable(const std::string& compoId,
                                        const std::string& variableId,
                                        const std::vector<double>& lb,
                                        double ub,
                                        bool integer,
                                        const Optimization::Dimensions& dim) const
{
    auto count = dim.getNumberOfTimesteps();
    if (lb.size() != count)
    {
        std::ostringstream errMessage;

        errMessage << "requested " << count << " variables but lb size = " << lb.size();
        throw BoundsSizeMismatch(errMessage.str());
    }
    optimEntityContainer_.addStartColumn();
    for (const auto& s: dim.getScenarioIndices())
    {
        for (const auto t: dim.getTimesteps())
        {
            auto year = buildOptional<Optimization::MCYearAndTime::MCYear>(
              dim.isScenarioDependent(),
              static_cast<Optimization::MCYearAndTime::MCYear>(s));
            const auto ts = buildOptional(dim.isTimeDependent(), t);
            auto localIndex = s * dim.getNumberOfTimesteps() + t;

            optimEntityContainer_.registerVariable(
              linear_problem_.addVariable(lb[t], /*use localIndex*/
                                          ub,
                                          integer,
                                          buildVariableName(compoId, variableId, year, ts)));
        }
    }
}

void VariablesBulkAddition::addVariable(const std::string& compoId,
                                        const std::string& variableId,
                                        double lb,
                                        const std::vector<double>& ub,
                                        bool integer,
                                        const Optimization::Dimensions& dim) const
{
    auto count = dim.getNumberOfTimesteps();
    if (ub.size() != count)
    {
        std::ostringstream errMessage;
        errMessage << "requested " << count << " variables but ub size = " << ub.size();
        throw BoundsSizeMismatch(errMessage.str());
    }
    optimEntityContainer_.addStartColumn();
    for (const auto& s: dim.getScenarioIndices())
    {
        for (const auto t: dim.getTimesteps())
        {
            auto year = buildOptional<Optimization::MCYearAndTime::MCYear>(
              dim.isScenarioDependent(),
              static_cast<Optimization::MCYearAndTime::MCYear>(s));
            const auto ts = buildOptional(dim.isTimeDependent(), t);
            auto localIndex = s * dim.getNumberOfTimesteps() + t;

            optimEntityContainer_.registerVariable(
              linear_problem_.addVariable(lb,
                                          ub[t], /*use localIndex*/
                                          integer,
                                          buildVariableName(compoId, variableId, year, ts)));
        }
    }
}

void VariablesBulkAddition::addVariable(const std::string& compoId,
                                        const std::string& variableId,
                                        const std::vector<double>& lb,
                                        const std::vector<double>& ub,
                                        bool integer,
                                        const Optimization::Dimensions& dim) const
{
    auto count = dim.getNumberOfTimesteps();
    if (lb.size() != ub.size() || lb.size() != count)
    {
        std::ostringstream errMessage;
        errMessage << "requested " << count << " variables but lb size = " << lb.size()
                   << " and ub size = " << ub.size();
        throw BoundsSizeMismatch(errMessage.str());
    }
    optimEntityContainer_.addStartColumn();
    for (const auto& s: dim.getScenarioIndices())
    {
        for (const auto t: dim.getTimesteps())
        {
            auto year = buildOptional<Optimization::MCYearAndTime::MCYear>(
              dim.isScenarioDependent(),
              static_cast<Optimization::MCYearAndTime::MCYear>(s));
            const auto ts = buildOptional(dim.isTimeDependent(), t);
            auto localIndex = s * dim.getNumberOfTimesteps() + t;
            optimEntityContainer_.registerVariable(
              linear_problem_.addVariable(lb[t], /*use localIndex*/
                                          ub[t], /*use localIndex*/
                                          integer,
                                          buildVariableName(compoId, variableId, year, ts)));
        }
    }
}

ComponentFiller::ComponentFiller(const ModelerStudy::SystemModel::Component& component,
                                 OptimEntityContainer& optimEntityContainer,
                                 const LinearProblemApi::ILinearProblemData& data,
                                 const ScenarioGroupRepository& scenarioGroupRepository):
    component_(component),
    optimEntityContainer_(optimEntityContainer),
    data_(data),
    scenarioGroupRepository_(scenarioGroupRepository)
{
}

bool checkTimeSteps(const LinearProblemApi::FillContext& ctx)
{
    return ctx.getLocalFirstTimeStep() <= ctx.getLocalLastTimeStep();
}

void ComponentFiller::addVariables(const LinearProblemApi::FillContext& ctx)
{
    if (!checkTimeSteps(ctx))
    {
        // exception?
        return;
    }

    const auto& evaluationContext = optimEntityContainer_.getOptimComponent(component_.Index())
                                      .evaluationContext;
    Expressions::Visitors::EvalVisitor evaluator(optimEntityContainer_, ctx, component_);
    auto valueOrDefault = [&evaluator](const auto& node, double defaultValue)
    {
        if (node.Empty())
        {
            return Expressions::Visitors::EvaluationResult(defaultValue);
        }
        return evaluator.dispatch(node.RootNode());
    };
    const auto& variables = component_.getModel()->Variables();
    auto& pb = optimEntityContainer_.Problem();
    for (auto i = 0; i < variables.size(); ++i)
    {
        const auto& variable = variables[i];
        namespace SM = ModelerStudy::SystemModel;
        const auto& lb = valueOrDefault(variable.LowerBound(),
                                        variable.Type() == SM::ValueType::BOOL ? 0
                                                                               : -pb.infinity());
        const auto& ub = valueOrDefault(variable.UpperBound(),
                                        variable.Type() == SM::ValueType::BOOL ? 1 : pb.infinity());
        if (variable.isTimeDependent())
        {
            const Optimization::Dimensions dim(
              Optimization::IntegerInterval{ctx.getYear(),
                                            ctx.getYear()}, /*TODO Handle range of year ? */
              Optimization::IntegerInterval(ctx.getLocalFirstTimeStep(),
                                            ctx.getLocalLastTimeStep()));
            // std::visit to handle the 4 cases: double/double, vector/double,
            // double/vector and vector/vector.
            std::visit(
              [&pb, &variable, this, &dim](const auto& lb_, const auto& ub_)
              {
                  VariablesBulkAddition(pb, optimEntityContainer_)
                    .addVariable(component_.Id(),
                                 variable.Id(),
                                 lb_,
                                 ub_,
                                 variable.Type() != ModelerStudy::SystemModel::ValueType::FLOAT,
                                 dim);
              },
              lb.value(),
              ub.value());
        }
        else
        {
            // No time component
            const Optimization::Dimensions dim({}, {});

            VariablesBulkAddition(pb, optimEntityContainer_)
              .addVariable(component_.Id(),
                           variable.Id(),
                           lb.valueAsDouble(),
                           ub.valueAsDouble(),
                           variable.Type() != ModelerStudy::SystemModel::ValueType::FLOAT,
                           dim);
        }
    }
}

void ComponentFiller::addStaticConstraint(const Optimization::LinearConstraint& linear_constraint,
                                          const std::string& constraint_id)
{
    auto* ct = optimEntityContainer_.Problem().addConstraint(linear_constraint.lb(0),
                                                             linear_constraint.ub(0),
                                                             component_.Id() + "." + constraint_id);

    optimEntityContainer_.registerConstraint(ct);
    const auto& solverVariables = optimEntityContainer_.getVariables();

    for (Eigen::SparseMatrix<double, Eigen::RowMajor>::InnerIterator
           it(linear_constraint.coef_per_var, 0);
         it;
         ++it)
    {
        ct->setCoefficient(solverVariables[it.col()], it.value());
    }
}

void ComponentFiller::addTimeDependentConstraints(
  const Optimization::LinearConstraint& linear_constraints,
  const std::string& constraint_id,
  const Optimisation::LinearProblemApi::FillContext& ctx)
{
    auto& pb = optimEntityContainer_.Problem();
    const Optimization::Dimensions dim(
      Optimization::IntegerInterval{ctx.getYear(), ctx.getYear()}, /*TODO Handle range of year ? */
      Optimization::IntegerInterval(ctx.getLocalFirstTimeStep(), ctx.getLocalLastTimeStep()));

    const auto& solverVariables = optimEntityContainer_.getVariables();
    for (const auto s: dim.getScenarioIndices()) // TODO
    {
        for (const auto t: dim.getTimesteps())
        {
            auto* ct = pb.addConstraint(linear_constraints.lb(t),
                                        linear_constraints.ub(t),
                                        component_.Id() + "." + constraint_id + '_'
                                          + std::to_string(t));
            optimEntityContainer_.registerConstraint(ct);

            for (Eigen::SparseMatrix<double, Eigen::RowMajor>::InnerIterator
                   it(linear_constraints.coef_per_var, t);
                 it;
                 ++it)
            {
                ct->setCoefficient(solverVariables[it.col()], it.value());
            }
        }
    }
}

void ComponentFiller::addConstraints(const LinearProblemApi::FillContext& ctx)
{
    Optimization::ReadLinearConstraintVisitor visitor(ctx, component_, optimEntityContainer_);

    const auto& modelConstraints = component_.getModel()->Constraints();
    for (auto constraintLocalIndex = 0; constraintLocalIndex < modelConstraints.size();
         ++constraintLocalIndex)
    {
        const auto& constraint = modelConstraints[constraintLocalIndex];
        auto* root_node = constraint.expression().RootNode();
        auto linear_constraints = visitor.dispatch(root_node);
        const auto gLobalIndex = optimEntityContainer_.ConstraintGLobalIndex();
        auto& optimComponent = optimEntityContainer_.getOptimComponent(component_.Index());
        optimComponent.modelConstraintsGlobalIndices.push_back(gLobalIndex);
        const auto timeIndex = getConstraintTimeIndex(root_node, component_);
        optimComponent.modelConstraintsTimeIndex.push_back(timeIndex);

        optimEntityContainer_.IncrementConstraintGLobalIndex();
        optimEntityContainer_.addStartLine();
        if (timeIndex == TimeIndex::VARYING_IN_TIME_ONLY
            || timeIndex == TimeIndex::VARYING_IN_TIME_AND_SCENARIO)
        {
            addTimeDependentConstraints(linear_constraints, constraint.Id(), ctx);
        }
        else
        {
            addStaticConstraint(linear_constraints, constraint.Id());
        }
    }
}

void ComponentFiller::addObjective(const Optimisation::LinearProblemApi::FillContext& ctx)
{
    auto model = component_.getModel();
    if (model->Objective().Empty())
    {
        return;
    }

    const auto& solverVariables = optimEntityContainer_.getVariables();
    V::Visitor visitor(optimEntityContainer_, component_, ctx, 168); // TODO TimeSteps

    const auto linearExpression = visitor.dispatch(model->Objective().RootNode());

    auto& pb = optimEntityContainer_.Problem();
    for (const auto& expr: linearExpression)
    {
        for (const auto& [index, value]: expr.coefs)
        {
            pb.setObjectiveCoefficient(solverVariables[index], value);
        }
    }
}

TimeIndex ComponentFiller::getConstraintTimeIndex(
  const Expressions::Nodes::Node* node,
  const ModelerStudy::SystemModel::Component& component) const
{
    Expressions::Visitors::TimeIndexVisitor timeIndexVisitor(optimEntityContainer_, component);
    return timeIndexVisitor.dispatch(node);
}
} // namespace Antares::Optimisation
