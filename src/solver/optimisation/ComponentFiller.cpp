
#include "antares/solver/optimisation/ComponentFiller.h"

#include "antares/study/system-model/variable.h"

#include "../expressions/include/antares/solver/expressions/visitors/EvalVisitor.h"

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
    for (const std::pair<const std::string, Study::SystemModel::Variable>& var1:
         component_.getModel()->Variables())
    {
        pb.addVariable(evaluator.dispatch(var1.second.LowerBound().RootNode()),
                       evaluator.dispatch(var1.second.UpperBound().RootNode()),
                       var1.second.Type() != Study::SystemModel::ValueType::FLOAT,
                       var1.second.Id());
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
