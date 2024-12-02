
#include "antares/solver/optim-model-filler/ComponentFiller.h"

// TODO: remove this unnecessary includes
#include <ranges>

#include <antares/solver/expressions/nodes/ExpressionsNodes.h>
#include "antares/solver/expressions/visitors/EvalVisitor.h"
#include "antares/solver/optim-model-filler/ReadLinearConstraintVisitor.h"
#include "antares/study/system-model/variable.h"

namespace Antares::Optimization
{
ComponentFiller::ComponentFiller(const Study::SystemModel::Component& component):
    component_(component),
    evaluationContext_(component_.getParameterValues(), {})
{
}

void ComponentFiller::addVariables(Solver::Modeler::Api::ILinearProblem& pb,
                                   Solver::Modeler::Api::LinearProblemData& data,
                                   Solver::Modeler::Api::FillContext& ctx)
{
    std::unique_ptr<EvalVisitor> evaluator = std::make_unique<EvalVisitor>(evaluationContext_);
    for (const auto& variable: component_.getModel()->Variables() | std::views::values)
    {
        pb.addVariable(evaluator->dispatch(variable.LowerBound().RootNode()),
                       evaluator->dispatch(variable.UpperBound().RootNode()),
                       variable.Type() != Study::SystemModel::ValueType::FLOAT,
                       component_.Id() + "." + variable.Id());
    }
}

void ComponentFiller::addConstraints(Solver::Modeler::Api::ILinearProblem& pb,
                                     Solver::Modeler::Api::LinearProblemData& data,
                                     Solver::Modeler::Api::FillContext& ctx)
{
    ReadLinearConstraintVisitor visitor(evaluationContext_);
    for (const auto& constraint: component_.getModel()->getConstraints() | std::views::values)
    {
        auto linear_constraint = visitor.dispatch(constraint.expression().RootNode());
        auto ct = pb.addConstraint(linear_constraint.lb,
                                   linear_constraint.ub,
                                   component_.Id() + "." + constraint.Id());
        for (auto [var_id, coef]: linear_constraint.coef_per_var)
        {
            auto* variable = pb.getVariable(component_.Id() + "." + var_id);
            ct->setCoefficient(variable, coef);
        }
    }
}

void ComponentFiller::addObjective(Solver::Modeler::Api::ILinearProblem& pb,
                                   Solver::Modeler::Api::LinearProblemData& data,
                                   Solver::Modeler::Api::FillContext& ctx)
{
}

} // namespace Antares::Optimization
