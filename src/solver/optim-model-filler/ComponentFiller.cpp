// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include <ranges>
#include <stdexcept>
#include <variant>

#include <antares/exception/RuntimeError.hpp>
#include <antares/expressions/nodes/ExpressionsNodes.h>
#include <antares/expressions/visitors/EvalVisitor.h>
#include <antares/solver/optim-model-filler/ComponentFiller.h>
#include "antares/expressions/visitors/VariabilityVisitor.h"

namespace
{
template<typename T>
std::optional<T> buildOptional(bool condition, T value)
{
    if (condition)
    {
        return value;
    }
    return {};
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

void BendersDecomposition::setCurrentProblemId(std::string id)
{
    currentProblemId_ = id;
}

void BendersDecomposition::collectConnectionVariables(std::vector<std::string>&& varnames,
                                                      unsigned varsCountInPb)
{
    std::vector<std::string> names = std::move(varnames);
    unsigned nbVars = names.size();
    unsigned startIndexInPb = varsCountInPb - nbVars;
    unsigned varIndex = startIndexInPb;
    for (const auto& name: names)
    {
        connectionVars_[currentProblemId_].emplace_back(name, varIndex);
        varIndex++;
    }
}

BendersDecompositionWriter::BendersDecompositionWriter(const BendersDecomposition& bd):
    bd_(bd)
{
}

void BendersDecompositionWriter::write(std::ostream& os) const
{
    for (const auto& [problemId, v]: bd_.connections())
    {
        for (const auto& [variableName, variableIndex]: v)
        {
            os << problemId << '\t' << variableName << '\t' << variableIndex << '\n';
        }
    }
}

class AddVariableVisitor
{
public:
    AddVariableVisitor(const Variable& variable,
                       LinearProblemApi::ILinearProblem& linear_problem,
                       const VariableNames& variableNames,
                       const Dimensions& dimensions);

    void operator()(double lb, double ub) const;
    void operator()(const std::vector<double>& lb, double ub) const;
    void operator()(double lb, const std::vector<double>& ub) const;
    void operator()(const std::vector<double>& lb, const std::vector<double>& ub) const;

    class BoundsSizeMismatch: public std::invalid_argument
    {
        using std::invalid_argument::invalid_argument;
    };

private:
    const bool isInteger_;
    LinearProblemApi::ILinearProblem& linear_problem_;
    const VariableNames& variableNames_;
    const Dimensions& dims_;
};

AddVariableVisitor::AddVariableVisitor(const Variable& variable,
                                       LinearProblemApi::ILinearProblem& linear_problem,
                                       const VariableNames& variableNames,
                                       const Dimensions& dimensions):
    isInteger_(variable.Type() != ValueType::FLOAT),
    linear_problem_(linear_problem),
    variableNames_(variableNames),
    dims_(dimensions)
{
}

void AddVariableVisitor::operator()(double lb, double ub) const
{
    unsigned index = 0;
    for (const auto& s: dims_.getScenarioIndices())
    {
        for (const auto t: dims_.getTimesteps())
        {
            linear_problem_.addVariable(lb, ub, isInteger_, variableNames_.name(index));
            index++;
        }
    }
}

void AddVariableVisitor::operator()(const std::vector<double>& lb, double ub) const
{
    auto count = dims_.getNumberOfTimesteps();
    if (lb.size() != count)
    {
        std::ostringstream errMessage;

        errMessage << "requested " << count << " variables but lb size = " << lb.size();
        throw BoundsSizeMismatch(errMessage.str());
    }

    unsigned index = 0;
    for (const auto& s: dims_.getScenarioIndices())
    {
        for (const auto t: dims_.getTimesteps())
        {
            linear_problem_.addVariable(lb[t], ub, isInteger_, variableNames_.name(index));
            index++;
        }
    }
}

void AddVariableVisitor::operator()(double lb, const std::vector<double>& ub) const
{
    auto count = dims_.getNumberOfTimesteps();
    if (ub.size() != count)
    {
        std::ostringstream errMessage;
        errMessage << "requested " << count << " variables but ub size = " << ub.size();
        throw BoundsSizeMismatch(errMessage.str());
    }

    unsigned index = 0;
    for (const auto& s: dims_.getScenarioIndices())
    {
        for (const auto t: dims_.getTimesteps())
        {
            linear_problem_.addVariable(lb, ub[t], isInteger_, variableNames_.name(index));
            index++;
        }
    }
}

void AddVariableVisitor::operator()(const std::vector<double>& lb,
                                    const std::vector<double>& ub) const
{
    auto count = dims_.getNumberOfTimesteps();
    if (lb.size() != ub.size() || lb.size() != count)
    {
        std::ostringstream errMessage;
        errMessage << "requested " << count << " variables but lb size = " << lb.size()
                   << " and ub size = " << ub.size();
        throw BoundsSizeMismatch(errMessage.str());
    }

    unsigned index = 0;
    for (const auto& s: dims_.getScenarioIndices())
    {
        for (const auto t: dims_.getTimesteps())
        {
            linear_problem_.addVariable(lb[t], ub[t], isInteger_, variableNames_.name(index));
            index++;
        }
    }
}

ComponentFiller::ComponentFiller(const Component& component,
                                 OptimEntityContainer& optimEntityContainer,
                                 const ScenarioGroupRepository& scenarioGroupRepository,
                                 Solver::Config::Location targetLocation,
                                 BendersDecomposition* bendersDecomposition):
    component_(component),
    optimEntityContainer_(optimEntityContainer),
    scenarioGroupRepository_(scenarioGroupRepository),
    targetLocation_(targetLocation),
    bendersDecomposition_(bendersDecomposition)
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

    Visitors::EvalVisitor evaluator(optimEntityContainer_, ctx, component_);
    auto valueOrDefault = [&evaluator](const auto& node, double defaultValue)
    {
        if (node.Empty())
        {
            return Visitors::EvaluationResult(defaultValue);
        }
        return evaluator.dispatch(node.RootNode());
    };

    const auto& variables = component_.getModel()->Variables();
    auto& pb = optimEntityContainer_.Problem();

    for (const auto& variable: variables | locationFilter())
    {
        const auto& lb = valueOrDefault(variable.LowerBound(),
                                        variable.Type() == ValueType::BOOL ? 0 : -pb.infinity());
        const auto& ub = valueOrDefault(variable.UpperBound(),
                                        variable.Type() == ValueType::BOOL ? 1 : pb.infinity());

        optimEntityContainer_.addStartColumn();

        const auto dims = getDimensions(variable, ctx);
        VariableNames variableNames;
        variableNames.makeNames(component_, variable, dims);

        AddVariableVisitor addVariableVisitor(variable, pb, variableNames, dims);
        if (variable.isTimeDependent())
        {
            std::visit(addVariableVisitor, lb.value(), ub.value());
        }
        else
        {
            addVariableVisitor(lb.valueAsDouble(), ub.valueAsDouble());
        }

        // Add common variables
        if (bendersDecomposition_
            && variable.location() == Solver::Config::Location::MASTER_AND_SUBPROBLEMS)
        {
            bendersDecomposition_->collectConnectionVariables(variableNames.names(),
                                                              pb.variableCount());
        }
    }
}

void ComponentFiller::addStaticConstraint(const LinearConstraint& linear_constraint,
                                          const std::string& constraint_id) const
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
                                                  const LinearProblemApi::FillContext& ctx) const
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
    for (const auto& constraint: contraints | locationFilter())
    {
        auto* root_node = constraint.expression().RootNode();
        auto linear_constraints = visitor.dispatch(root_node);
        const auto variability = getVariability(root_node, component_);

        optimEntityContainer_.registerConstraint(component_, variability);

        if (isTimeDependent(variability))
        {
            addTimeDependentConstraints(linear_constraints, constraint.Id(), ctx);
        }
        else
        {
            addStaticConstraint(linear_constraints, constraint.Id());
        }
    }
}

void ComponentFiller::addStaticObjective(const Optimization::LinearExpression& expression) const
{
    auto& pb = optimEntityContainer_.Problem();
    const auto& solverVariables = optimEntityContainer_.getVariables();

    for (const auto& [index, value]: expression)
    {
        pb.setObjectiveCoefficient(solverVariables[index].get(), value);
    }
}

void ComponentFiller::addObjectives(const LinearProblemApi::FillContext& ctx)
{
    auto* model = component_.getModel();
    ReadLinearExpressionVisitor visitor(optimEntityContainer_, ctx, component_);

    double objectiveOffset = 0.0;
    for (const auto& objective: model->Objectives() | locationFilter())
    {
        const auto root_node = objective.expression().RootNode();
        const auto variability = getVariability(root_node, component_);
        if (isTimeDependent(variability))
        {
            throw Error::RuntimeError("Time dependent objectives are not supported in Antares.");
        }
        const auto linearExpression = visitor.visitMergeDuplicates(root_node)[0];
        addStaticObjective(linearExpression);
        objectiveOffset += linearExpression.constant();
    }
    auto& pb = optimEntityContainer_.Problem();
    pb.setObjectiveOffset(pb.getObjectiveOffset() + objectiveOffset);
}

VariabilityType ComponentFiller::getVariability(const Node* node, const Component& component) const
{
    Visitors::VariabilityVisitor variability_visitor(optimEntityContainer_, component);
    return variability_visitor.dispatch(node);
}
} // namespace Antares::Optimisation
