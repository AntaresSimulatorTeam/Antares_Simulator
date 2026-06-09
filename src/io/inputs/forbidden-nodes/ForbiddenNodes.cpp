// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include <antares/expressions/nodes/ExpressionsNodes.h>
#include <antares/io/inputs/forbidden-nodes/ForbiddenNodes.h>

namespace Antares::IO::Inputs::ForbidNodes
{
bool ForbiddenNodes::isGloballyForbidden(const std::type_index& typeId) const
{
    return global_.contains(typeId);
}

bool ForbiddenNodes::isForbiddenByParent(const std::type_index& parentTypeId,
                                         const std::type_index& nodeTypeId) const
{
    const auto& it = rules_.find(parentTypeId);
    return (it != rules_.end()) && it->second.contains(nodeTypeId);
}

using namespace Antares::Expressions::Nodes;

void forbidVariablesInFunctionNodes(ForbiddenNodes& f)
{
    f.parentForbidsChild<FunctionNodeType::max, VariableNode>();
    f.parentForbidsChild<FunctionNodeType::min, VariableNode>();
    f.parentForbidsChild<FunctionNodeType::floor, VariableNode>();
    f.parentForbidsChild<FunctionNodeType::ceil, VariableNode>();
}

void forbidPortFieldsInFunctionNodes(ForbiddenNodes& f)
{
    // max(...) : fordidding children
    f.parentForbidsChild<FunctionNodeType::max, PortFieldNode>();
    f.parentForbidsChild<FunctionNodeType::max, PortFieldSumNode>();

    // min(...) : fordidding children
    f.parentForbidsChild<FunctionNodeType::min, PortFieldNode>();
    f.parentForbidsChild<FunctionNodeType::min, PortFieldSumNode>();

    // floor(node) : fordidding children
    f.parentForbidsChild<FunctionNodeType::floor, PortFieldNode>();
    f.parentForbidsChild<FunctionNodeType::floor, PortFieldSumNode>();

    // ceil(node) : fordidding children
    f.parentForbidsChild<FunctionNodeType::ceil, PortFieldNode>();
    f.parentForbidsChild<FunctionNodeType::ceil, PortFieldSumNode>();
}

void ForbidInFunctionNodes(ForbiddenNodes& f)
{
    forbidVariablesInFunctionNodes(f);
    forbidPortFieldsInFunctionNodes(f);
}

void ForbidConstraintSignNodes(ForbiddenNodes& f)
{
    f.forbidGlobally<ComparisonNode, EqualNode, LessThanOrEqualNode, GreaterThanOrEqualNode>();
}

ForbiddenNodes ForbidNodesInConstraint()
{
    ForbiddenNodes f;
    ForbidInFunctionNodes(f);
    f.forbidGlobally<PortFieldSumNode>();
    f.forbidGlobally<FunctionNodeType::reduced_cost, FunctionNodeType::dual>();
    return f;
}

ForbiddenNodes ForbidNodesInBindingConstraint()
{
    ForbiddenNodes f;
    ForbidInFunctionNodes(f);
    f.forbidGlobally<FunctionNodeType::reduced_cost, FunctionNodeType::dual>();
    return f;
}

ForbiddenNodes ForbidNodesInVariableBounds()
{
    ForbiddenNodes f;
    ForbidInFunctionNodes(f);
    ForbidConstraintSignNodes(f);
    f.forbidGlobally<PortFieldSumNode>();
    f.forbidGlobally<FunctionNodeType::reduced_cost, FunctionNodeType::dual>();
    return f;
}

ForbiddenNodes ForbidNodesInPortFieldDef()
{
    ForbiddenNodes f;
    forbidPortFieldsInFunctionNodes(f);
    ForbidConstraintSignNodes(f);
    f.forbidGlobally<PortFieldSumNode>();
    return f;
}

ForbiddenNodes ForbidNodesInObjective()
{
    ForbiddenNodes f;
    ForbidInFunctionNodes(f);
    ForbidConstraintSignNodes(f);
    f.forbidGlobally<PortFieldSumNode>();
    f.forbidGlobally<FunctionNodeType::reduced_cost, FunctionNodeType::dual>();
    return f;
}

ForbiddenNodes ForbidNodesInExtraOutput()
{
    return {}; // Nothing is forbidden
}

static ForbiddenNodes ForbidNonLinearNodes()
{
    ForbiddenNodes f;
    f.forbidGlobally<FunctionNodeType::reduced_cost,
                     FunctionNodeType::dual,
                     FunctionNodeType::max,
                     FunctionNodeType::min,
                     FunctionNodeType::floor,
                     FunctionNodeType::ceil,
                     FunctionNodeType::pow>();
    return f;
}

const ForbiddenNodes forbiddenInConstraint = ForbidNodesInConstraint();
const ForbiddenNodes forbiddenInBindingConstraint = ForbidNodesInBindingConstraint();
const ForbiddenNodes forbiddenInVariableBounds = ForbidNodesInVariableBounds();
const ForbiddenNodes forbiddenInPortFieldDef = ForbidNodesInPortFieldDef();
const ForbiddenNodes forbiddenInObjective = ForbidNodesInObjective();
const ForbiddenNodes forbiddenInExtraOutput = ForbidNodesInExtraOutput();
const ForbiddenNodes forbidNonLinearNodes = ForbidNonLinearNodes();

} // namespace Antares::IO::Inputs::ForbidNodes
