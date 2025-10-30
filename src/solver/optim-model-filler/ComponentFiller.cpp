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
#include "antares/expressions/visitors/TimeIndexVisitor.h"

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
} // namespace

using namespace Antares::Expressions;
using namespace Antares::Expressions::Nodes;
using namespace Antares::ModelerStudy::SystemModel;

namespace Antares::Optimisation
{

class VariableNames
{
public:
    VariableNames() = default;
    void makeNames(const Component& compo, const Variable& var, const Dimensions& dims);
    std::string name(unsigned index) const;
    std::vector<std::string> names();

private:
    std::vector<std::string> names_;
};

void VariableNames::makeNames(const Component& compo, const Variable& var, const Dimensions& dims)
{
    unsigned index = 0;
    names_.resize(dims.getScenarioIndices().size() * dims.getTimesteps().size());
    for (const auto& s: dims.getScenarioIndices())
    {
        for (const auto t: dims.getTimesteps())
        {
            auto year = buildOptional(dims.isScenarioDependent(),
                                      static_cast<Optimization::MCYearAndTime::MCYear>(s));
            const auto ts = buildOptional(dims.isTimeDependent(), t);
            std::string name = buildVariableName(compo.Id(), var.Id(), year, ts);
            names_[index] = name;
            index++;
        }
    }
}

std::string VariableNames::name(unsigned index) const
{
    return names_[index];
}

std::vector<std::string> VariableNames::names()
{
    return names_;
}

void MasterAndSubPbVariables::setProblemIdentifier(std::string id)
{
    pbIdentifier_ = id;
}

void MasterAndSubPbVariables::add(std::vector<std::string>&& varnames, unsigned varsCountInPb)
{
    std::vector<std::string> names = std::move(varnames);
    unsigned nbVars = names.size();
    unsigned startIndexInPb = varsCountInPb - nbVars;
    unsigned varIndex = startIndexInPb;
    for (const auto& name: names)
    {
        masterAndSubPbVars_[pbIdentifier_].emplace_back(name, varIndex);
        varIndex++;
    }
}

class VariablesBulkAddition
{
public:
    VariablesBulkAddition(LinearProblemApi::ILinearProblem& linear_problem,
                          const VariableNames& variableNames);

    void addVariable(double lb, double ub, bool integer, const Dimensions& dim) const;

    void addVariable(const std::vector<double>& lb,
                     double ub,
                     bool integer,
                     const Dimensions& dim) const;

    void addVariable(double lb,
                     const std::vector<double>& ub,
                     bool integer,
                     const Dimensions& dim) const;

    void addVariable(const std::vector<double>& lb,
                     const std::vector<double>& ub,
                     bool integer,
                     const Dimensions& dim) const;

    class BoundsSizeMismatch: public std::invalid_argument
    {
        using std::invalid_argument::invalid_argument;
    };

private:
    LinearProblemApi::ILinearProblem& linear_problem_;
    const VariableNames& variableNames_;
};

VariablesBulkAddition::VariablesBulkAddition(LinearProblemApi::ILinearProblem& linear_problem,
                                             const VariableNames& variableNames):
    linear_problem_(linear_problem),
    variableNames_(variableNames)
{
}

void VariablesBulkAddition::addVariable(double lb,
                                        double ub,
                                        bool integer,
                                        const Dimensions& dim) const
{
    unsigned index = 0;
    for (const auto& s: dim.getScenarioIndices())
    {
        for (const auto t: dim.getTimesteps())
        {
            linear_problem_.addVariable(lb, ub, integer, variableNames_.name(index));
            index++;
        }
    }
}

void VariablesBulkAddition::addVariable(const std::vector<double>& lb,
                                        double ub,
                                        bool integer,
                                        const Dimensions& dim) const
{
    auto count = dim.getNumberOfTimesteps();
    if (lb.size() != count)
    {
        std::ostringstream errMessage;

        errMessage << "requested " << count << " variables but lb size = " << lb.size();
        throw BoundsSizeMismatch(errMessage.str());
    }

    unsigned index = 0;
    for (const auto& s: dim.getScenarioIndices())
    {
        for (const auto t: dim.getTimesteps())
        {
            linear_problem_.addVariable(lb[t], ub, integer, variableNames_.name(index));
            index++;
        }
    }
}

void VariablesBulkAddition::addVariable(double lb,
                                        const std::vector<double>& ub,
                                        bool integer,
                                        const Dimensions& dim) const
{
    auto count = dim.getNumberOfTimesteps();
    if (ub.size() != count)
    {
        std::ostringstream errMessage;
        errMessage << "requested " << count << " variables but ub size = " << ub.size();
        throw BoundsSizeMismatch(errMessage.str());
    }

    unsigned index = 0;
    for (const auto& s: dim.getScenarioIndices())
    {
        for (const auto t: dim.getTimesteps())
        {
            linear_problem_.addVariable(lb, ub[t], integer, variableNames_.name(index));
            index++;
        }
    }
}

void VariablesBulkAddition::addVariable(const std::vector<double>& lb,
                                        const std::vector<double>& ub,
                                        bool integer,
                                        const Dimensions& dim) const
{
    auto count = dim.getNumberOfTimesteps();
    if (lb.size() != ub.size() || lb.size() != count)
    {
        std::ostringstream errMessage;
        errMessage << "requested " << count << " variables but lb size = " << lb.size()
                   << " and ub size = " << ub.size();
        throw BoundsSizeMismatch(errMessage.str());
    }

    unsigned index = 0;
    for (const auto& s: dim.getScenarioIndices())
    {
        for (const auto t: dim.getTimesteps())
        {
            linear_problem_.addVariable(lb[t], ub[t], integer, variableNames_.name(index));
            index++;
        }
    }
}

ComponentFiller::ComponentFiller(const ModelerStudy::SystemModel::Component& component,
                                 OptimEntityContainer& optimEntityContainer,
                                 const ScenarioGroupRepository& scenarioGroupRepository,
                                 Modeler::Config::Location targetLocation,
                                 MasterAndSubPbVariables* masterAndSubPbvars):
    component_(component),
    optimEntityContainer_(optimEntityContainer),
    scenarioGroupRepository_(scenarioGroupRepository),
    targetLocation_(targetLocation),
    masterAndSubPbvars_(masterAndSubPbvars)
{
}

bool checkTimeSteps(const LinearProblemApi::FillContext& ctx)
{
    return ctx.getLocalFirstTimeStep() <= ctx.getLocalLastTimeStep();
}

Dimensions getDimensions(const LinearProblemApi::FillContext& ctx)
{
    Dimensions dims(IntegerInterval{ctx.getYear(), ctx.getYear()},
                    IntegerInterval(ctx.getLocalFirstTimeStep(), ctx.getLocalLastTimeStep()));
    return dims;
}

Dimensions getDimensions(const Variable& var, const LinearProblemApi::FillContext& ctx)
{
    if (!var.isTimeDependent())
    {
        return {{}, {}};
    }
    return getDimensions(ctx);
}

void ComponentFiller::addVariables(const LinearProblemApi::FillContext& ctx)
{
    if (!checkTimeSteps(ctx))
    {
        // exception?
        return;
    }

    const auto& evaluationContext = optimEntityContainer_.getEvaluationContext(component_);
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
    for (const auto& variable: variables)
    {
        // Skip the variable in case of location mismatch
        if (!AreLocationsCompatible(variable.location(), targetLocation_))
        {
            continue;
        }
        const auto& lb = valueOrDefault(variable.LowerBound(),
                                        variable.Type() == ValueType::BOOL ? 0 : -pb.infinity());
        const auto& ub = valueOrDefault(variable.UpperBound(),
                                        variable.Type() == ValueType::BOOL ? 1 : pb.infinity());

        optimEntityContainer_.addStartColumn();

        const auto dims = getDimensions(variable, ctx);
        VariableNames variableNames;
        variableNames.makeNames(component_, variable, dims);

        if (variable.isTimeDependent())
        {
            // std::visit to handle the 4 cases: double/double, vector/double,
            // double/vector and vector/vector.
            std::visit(
              [&pb, &variable, this, &dims, &variableNames](const auto& lb_, const auto& ub_)
              {
                  VariablesBulkAddition(pb, variableNames)
                    .addVariable(lb_, ub_, variable.Type() != ValueType::FLOAT, dims);
              },
              lb.value(),
              ub.value());
        }
        else
        {
            VariablesBulkAddition(pb, variableNames)
              .addVariable(lb.valueAsDouble(),
                           ub.valueAsDouble(),
                           variable.Type() != ValueType::FLOAT,
                           dims);
        }

        if (variable.location() == Modeler::Config::Location::MASTER_AND_SUBPROBLEMS
            && masterAndSubPbvars_)
        {
            masterAndSubPbvars_->add(variableNames.names(), pb.variableCount());
        }
    }
}

void ComponentFiller::addStaticConstraint(const LinearConstraint& linear_constraint,
                                          const std::string& constraint_id)
{
    auto* ct = optimEntityContainer_.Problem().addConstraint(linear_constraint.lb[0],
                                                             linear_constraint.ub[0],
                                                             component_.Id() + "." + constraint_id);

    const auto& solverVariables = optimEntityContainer_.getVariables();
    const auto& coefsPerVar = linear_constraint.coef_per_var[0];

    for (const auto& [index, value]: coefsPerVar)
    {
        ct->setCoefficient(solverVariables[index].get(), value);
    }
}

void ComponentFiller::addTimeDependentConstraints(const LinearConstraint& linear_constraints,
                                                  const std::string& constraint_id,
                                                  const LinearProblemApi::FillContext& ctx)
{
    auto& pb = optimEntityContainer_.Problem();
    const auto dims = getDimensions(ctx);

    const auto& solverVariables = optimEntityContainer_.getVariables();
    for (const auto s: dims.getScenarioIndices()) // TODO
    {
        for (const auto t: dims.getTimesteps())
        {
            auto* ct = pb.addConstraint(linear_constraints.lb[t],
                                        linear_constraints.ub[t],
                                        component_.Id() + "." + constraint_id + '_'
                                          + std::to_string(t));

            const auto& coefsPerVar = linear_constraints.coef_per_var[t];
            for (const auto& [index, value]: coefsPerVar)
            {
                ct->setCoefficient(solverVariables[index].get(), value);
            }
        }
    }
}

void ComponentFiller::addConstraints(const LinearProblemApi::FillContext& ctx)
{
    ReadLinearConstraintVisitor visitor(optimEntityContainer_, ctx, component_);

    const auto& contraints = component_.getModel()->Constraints();
    for (const auto& constraint: contraints)
    {
        auto* root_node = constraint.expression().RootNode();
        auto linear_constraints = visitor.dispatch(root_node);
        const auto timeIndex = getConstraintTimeIndex(root_node, component_);

        optimEntityContainer_.registerConstraint(component_, timeIndex);

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

void ComponentFiller::addObjectives(const LinearProblemApi::FillContext& ctx)
{
    auto* model = component_.getModel();
    const auto& solverVariables = optimEntityContainer_.getVariables();
    ReadLinearExpressionVisitor visitor(optimEntityContainer_, ctx, component_);

    for (const auto& objective: model->Objectives())
    {
        // Skip the objective in case of location mismatch
        if (!AreLocationsCompatible(objective.location(), targetLocation_))
        {
            continue;
        }
        const auto linearExpression = visitor.visitMergeDuplicates(
          objective.expression().RootNode());

        auto& pb = optimEntityContainer_.Problem();
        for (const auto& expr: linearExpression)
        {
            for (const auto& [index, value]: expr)
            {
                pb.setObjectiveCoefficient(solverVariables[static_cast<std::size_t>(index)].get(),
                                           value);
            }
        }
    }
}

TimeIndex ComponentFiller::getConstraintTimeIndex(const Nodes::Node* node,
                                                  const Component& component) const
{
    Expressions::Visitors::TimeIndexVisitor timeIndexVisitor(optimEntityContainer_, component);
    return timeIndexVisitor.dispatch(node);
}
} // namespace Antares::Optimisation
