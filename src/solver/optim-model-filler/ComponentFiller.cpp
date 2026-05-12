// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include <ranges>
#include <stdexcept>
#include <variant>

#include <antares/exception/RuntimeError.hpp>
#include <antares/expressions/nodes/ExpressionsNodes.h>
#include <antares/expressions/visitors/EvalVisitor.h>
#include <antares/logs/logs.h>
#include <antares/solver/optim-model-filler/ComponentFiller.h>
#include <antares/solver/optim-model-filler/outOfBoundsTimeShift.h>
#include "antares/expressions/visitors/VariabilityVisitor.h"

namespace
{
unsigned countActiveConstraintTimesteps(
  const Antares::ModelerStudy::SystemModel::Constraint& constraint,
  const Antares::Optimisation::LinearProblemApi::FillContext& ctx,
  const Antares::Optimisation::OptimEntityContainer& optimEntityContainer,
  const Antares::ModelerStudy::SystemModel::Component& component,
  const Antares::Optimisation::LinearProblemApi::ILinearProblemData* data,
  const Antares::Optimisation::ScenarioGroupRepository& scenarioGroupRepo)
{
    if (constraint.outOfBoundsProcessingMode()
        == Antares::ModelerStudy::SystemModel::OutOfBoundsProcessingMode::CYCLIC)
    {
        return static_cast<unsigned>(ctx.getLocalNumberOfTimeSteps());
    }

    const auto& scenario = scenarioGroupRepo.scenario(component.getScenarioGroupId());
    Visitors::EvalVisitor evalVisitor(optimEntityContainer, ctx, component, data, scenario);
    unsigned activeConstraintCount = 0;
    for (const auto timeStep: Antares::Optimisation::IntegerInterval{ctx.getLocalFirstTimeStep(),
                                                                     ctx.getLocalLastTimeStep()})
    {
        if (!Antares::Optimisation::hasOutOfBoundsTimeShift(constraint.expression().RootNode(),
                                                            timeStep,
                                                            ctx,
                                                            evalVisitor))
        {
            ++activeConstraintCount;
        }
    }
    return activeConstraintCount;
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
    const auto& scenarioIndices = dims.getScenarioIndices();
    const auto& timesteps = dims.getTimesteps();

    names_.resize(scenarioIndices.size() * timesteps.size());

    std::string baseVarName = fmt::format("{}.{}", compo.Id(), var.Id());
    std::string scenarizedVarName;
    std::string tsVarName;

    for (const auto& s: scenarioIndices)
    {
        scenarizedVarName = baseVarName;
        if (dims.isScenarioDependent())
        {
            auto year = static_cast<Optimization::MCYearAndTime::MCYear>(s);
            scenarizedVarName += "_s" + std::to_string(format_as(year));
        }

        for (const auto t: timesteps)
        {
            tsVarName = scenarizedVarName;
            if (dims.isTimeDependent())
            {
                tsVarName += "_t" + std::to_string(t);
            }

            names_[index] = tsVarName;
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
    for (std::size_t i = 0; i < dims_.getScenarioIndices().size(); ++i)
    {
        for (std::size_t j = 0; j < dims_.getTimesteps().size(); ++j)
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
    for (std::size_t i = 0; i < dims_.getScenarioIndices().size(); ++i)
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
    for (std::size_t i = 0; i < dims_.getScenarioIndices().size(); ++i)
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
    for (std::size_t i = 0; i < dims_.getScenarioIndices().size(); ++i)
    {
        for (const auto t: dims_.getTimesteps())
        {
            linear_problem_.addVariable(lb[t], ub[t], isInteger_, variableNames_.name(index));
            index++;
        }
    }
}

ComponentFiller::ComponentFiller(const Component& component,
                                 const LinearProblemApi::ILinearProblemData* data,
                                 OptimEntityContainer& optimEntityContainer,
                                 const ScenarioGroupRepository& scenarioGroupRepo,
                                 Solver::Config::Location targetLocation,
                                 BendersDecomposition* bendersDecomposition):
    component_(component),
    optimEntityContainer_(optimEntityContainer),
    pb_(optimEntityContainer_.Problem()),
    data_(data),
    scenarioGroupRepo_(scenarioGroupRepo),
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

    const auto& scenario = scenarioGroupRepo_.scenario(component_.getScenarioGroupId());
    Visitors::EvalVisitor evaluator(optimEntityContainer_, ctx, component_, data_, scenario);
    auto valueOrDefault = [&evaluator](const auto& node, double defaultValue)
    {
        if (node.Empty())
        {
            return Visitors::EvaluationResult(defaultValue);
        }
        return evaluator.dispatch(node.RootNode());
    };

    const auto& variables = component_.getModel()->Variables();

    for (const auto& variable: variables | locationFilter())
    {
        const auto& lb = valueOrDefault(variable.LowerBound(),
                                        variable.Type() == ValueType::BOOL ? 0 : -pb_.infinity());
        const auto& ub = valueOrDefault(variable.UpperBound(),
                                        variable.Type() == ValueType::BOOL ? 1 : pb_.infinity());

        optimEntityContainer_.addStartColumn();

        const auto dims = getDimensions(variable, ctx);
        VariableNames variableNames;
        variableNames.makeNames(component_, variable, dims);

        AddVariableVisitor addVariableVisitor(variable, pb_, variableNames, dims);
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
            bendersDecomposition_->collectCouplingVariables(variableNames.names(),
                                                            static_cast<unsigned>(
                                                              pb_.variableCount()));
        }
    }
}

void ComponentFiller::addStaticConstraint(const LinearConstraint& linear_constraint,
                                          const std::string& constraint_id) const
{
    auto* ct = pb_.addConstraint(linear_constraint.lb[0],
                                 linear_constraint.ub[0],
                                 component_.Id() + "." + constraint_id);

    const auto& solverVariables = pb_.getVariables();
    const auto& coefsPerVar = linear_constraint.coef_per_var[0];

    for (const auto& [index, value]: coefsPerVar)
    {
        ct->setCoefficient(solverVariables[index].get(), value);
    }
}

void ComponentFiller::addTimeDependentConstraints(const LinearConstraint& linear_constraints,
                                                  const std::string& constraint_id,
                                                  const LinearProblemApi::FillContext& ctx,
                                                  const Constraint& constraint) const
{
    const auto dims = getDimensions(ctx);

    const auto& solverVariables = pb_.getVariables();
    const auto firstTimestep = dims.getTimesteps().initialTime;

    const bool isDrop = constraint.outOfBoundsProcessingMode() == OutOfBoundsProcessingMode::DROP;

    // If DROP mode is enabled, construct a single EvalVisitor and iterate timesteps once.
    if (isDrop)
    {
        const auto& scenario = scenarioGroupRepo_.scenario(component_.getScenarioGroupId());
        Expressions::Visitors::EvalVisitor evalVisitor(optimEntityContainer_,
                                                       ctx,
                                                       component_,
                                                       data_,
                                                       scenario);

        for (const auto t: dims.getTimesteps())
        {
            const auto localIndex = static_cast<std::size_t>(t - firstTimestep);
            if (Antares::Optimisation::hasOutOfBoundsTimeShift(constraint.expression().RootNode(),
                                                               t,
                                                               ctx,
                                                               evalVisitor))
            {
                logs.debug() << "Constraint " << constraint_id
                             << " was ignored because it is marked as DROP for t = " << t;
                continue;
            }

            auto* ct = pb_.addConstraint(linear_constraints.lb[localIndex],
                                         linear_constraints.ub[localIndex],
                                         component_.Id() + "." + constraint_id + '_'
                                           + std::to_string(t));

            const auto& coefsPerVar = linear_constraints.coef_per_var[localIndex];
            for (const auto& [index, value]: coefsPerVar)
            {
                ct->setCoefficient(solverVariables[index].get(), value);
            }
        }
    }
    else
    {
        for (const auto t: dims.getTimesteps())
        {
            const auto localIndex = static_cast<std::size_t>(t - firstTimestep);
            auto* ct = pb_.addConstraint(linear_constraints.lb[localIndex],
                                         linear_constraints.ub[localIndex],
                                         component_.Id() + "." + constraint_id + '_'
                                           + std::to_string(t));

            const auto& coefsPerVar = linear_constraints.coef_per_var[localIndex];
            for (const auto& [index, value]: coefsPerVar)
            {
                ct->setCoefficient(solverVariables[index].get(), value);
            }
        }
    }
}

void ComponentFiller::addConstraints(const LinearProblemApi::FillContext& ctx)
{
    const auto& contraints = component_.getModel()->Constraints();
    ReadLinearConstraintVisitor visitor(optimEntityContainer_,
                                        ctx,
                                        component_,
                                        data_,
                                        scenarioGroupRepo_);

    for (const auto& constraint: contraints | locationFilter())
    {
        auto* root_node = constraint.expression().RootNode();
        auto linear_constraints = visitor.dispatch(root_node);
        const auto variability = getVariability(root_node, component_);

        unsigned activeConstraintCount = 1;
        if (isTimeDependent(variability))
        {
            activeConstraintCount = countActiveConstraintTimesteps(constraint,
                                                                   ctx,
                                                                   optimEntityContainer_,
                                                                   component_,
                                                                   data_,
                                                                   scenarioGroupRepo_);
        }

        optimEntityContainer_.registerConstraint(component_, variability, activeConstraintCount);

        if (isTimeDependent(variability))
        {
            addTimeDependentConstraints(linear_constraints, constraint.Id(), ctx, constraint);
        }
        else
        {
            addStaticConstraint(linear_constraints, constraint.Id());
        }
    }
}

void ComponentFiller::addStaticObjective(const Optimization::LinearExpression& expression) const
{
    const auto& solverVariables = pb_.getVariables();

    for (const auto& [index, value]: expression)
    {
        pb_.setObjectiveCoefficient(solverVariables[index].get(), value);
    }
}

void ComponentFiller::addObjectives(const LinearProblemApi::FillContext& ctx)
{
    auto* model = component_.getModel();
    ReadLinearExpressionVisitor visitor(optimEntityContainer_,
                                        ctx,
                                        component_,
                                        data_,
                                        scenarioGroupRepo_);

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
    pb_.setObjectiveOffset(pb_.getObjectiveOffset() + objectiveOffset);
}

VariabilityType ComponentFiller::getVariability(const Node* node, const Component& component) const
{
    Visitors::VariabilityVisitor variability_visitor(optimEntityContainer_, component);
    return variability_visitor.dispatch(node);
}
} // namespace Antares::Optimisation
