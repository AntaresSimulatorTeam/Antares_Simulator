
#include "antares/solver/optim-model-filler/ComponentFiller.h"

// TODO: remove this unnecessary includes
#include <antares/solver/expressions/nodes/ExpressionsNodes.h>
#include "antares/solver/expressions/visitors/EvalVisitor.h"
#include "antares/study/system-model/variable.h"

namespace Antares::Optimization
{
ComponentFiller::ComponentFiller(const Study::SystemModel::Component& component):
    component_(component)
{
}

void ComponentFiller::addVariables(Solver::Modeler::Api::ILinearProblem& pb,
                                   Solver::Modeler::Api::LinearProblemData& data,
                                   Solver::Modeler::Api::FillContext& ctx)
{
    Solver::Visitors::EvalVisitor evaluator;
    for (const auto& variable: component_.getModel()->Variables() | std::views::values)
    {
        pb.addVariable(evaluator.dispatch(variable.LowerBound().RootNode()),
                       evaluator.dispatch(variable.UpperBound().RootNode()),
                       variable.Type() != Study::SystemModel::ValueType::FLOAT,
                       component_.Id() + "." + variable.Id());
    }
}

void ComponentFiller::addConstraints(Solver::Modeler::Api::ILinearProblem& pb,
                                     Solver::Modeler::Api::LinearProblemData& data,
                                     Solver::Modeler::Api::FillContext& ctx)
{
}

void ComponentFiller::addObjective(Solver::Modeler::Api::ILinearProblem& pb,
                                   Solver::Modeler::Api::LinearProblemData& data,
                                   Solver::Modeler::Api::FillContext& ctx)
{
}

} // namespace Antares::Optimization
