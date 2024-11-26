
#include "antares/solver/optim-model-filler/ComponentFiller.h"

// TODO: remove this unnecessary includes
#include "antares/solver/expressions/nodes/ComparisonNode.h"
#include "antares/solver/expressions/nodes/ComponentNode.h"
#include "antares/solver/expressions/nodes/DivisionNode.h"
#include "antares/solver/expressions/nodes/EqualNode.h"
#include "antares/solver/expressions/nodes/GreaterThanOrEqualNode.h"
#include "antares/solver/expressions/nodes/LessThanOrEqualNode.h"
#include "antares/solver/expressions/nodes/LiteralNode.h"
#include "antares/solver/expressions/nodes/MultiplicationNode.h"
#include "antares/solver/expressions/nodes/NegationNode.h"
#include "antares/solver/expressions/nodes/ParameterNode.h"
#include "antares/solver/expressions/nodes/PortFieldNode.h"
#include "antares/solver/expressions/nodes/PortFieldSumNode.h"
#include "antares/solver/expressions/nodes/SubtractionNode.h"
#include "antares/solver/expressions/nodes/SumNode.h"
#include "antares/solver/expressions/nodes/VariableNode.h"
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
    for (const std::pair<const std::string, Study::SystemModel::Variable>& var1:
         component_.getModel()->Variables())
    {
        pb.addVariable(evaluator.dispatch(var1.second.LowerBound().RootNode()),
                       evaluator.dispatch(var1.second.UpperBound().RootNode()),
                       var1.second.Type() != Study::SystemModel::ValueType::FLOAT,
                       component_.Id() + "." + var1.second.Id());
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
