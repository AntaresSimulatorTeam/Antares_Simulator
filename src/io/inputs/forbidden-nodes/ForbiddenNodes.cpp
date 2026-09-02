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
    f.parentForbidsChild<FunctionNodeType::round, VariableNode>();
    f.parentForbidsChild<FunctionNodeType::abs, VariableNode>();
}

void forbidPortFieldsInFunctionNodes(ForbiddenNodes& f)
{
    // max(...) : fordiding children
    f.parentForbidsChild<FunctionNodeType::max, PortFieldNode>();
    f.parentForbidsChild<FunctionNodeType::max, PortFieldSumNode>();

    // min(...) : fordiding children
    f.parentForbidsChild<FunctionNodeType::min, PortFieldNode>();
    f.parentForbidsChild<FunctionNodeType::min, PortFieldSumNode>();

    // floor(node) : fordiding children
    f.parentForbidsChild<FunctionNodeType::floor, PortFieldNode>();
    f.parentForbidsChild<FunctionNodeType::floor, PortFieldSumNode>();

    // ceil(node) : fordiding children
    f.parentForbidsChild<FunctionNodeType::ceil, PortFieldNode>();
    f.parentForbidsChild<FunctionNodeType::ceil, PortFieldSumNode>();

    // round(node) : fordiding children
    f.parentForbidsChild<FunctionNodeType::round, PortFieldNode>();
    f.parentForbidsChild<FunctionNodeType::round, PortFieldSumNode>();

    // abs(node) : fordiding children
    f.parentForbidsChild<FunctionNodeType::abs, PortFieldNode>();
    f.parentForbidsChild<FunctionNodeType::abs, PortFieldSumNode>();
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

// Used to check linearity of binding constraints behind connections (and of the port field
// definitions resolved through them).
// Non-linear function nodes (max, min, floor, ceil, round, abs, pow) are not globally
// forbidden : they are allowed as long as they do not take variables (directly or
// indirectly) as arguments, since in that case they only depend on parameters and
// literals, which evaluate to constants before the linear optimization.
// reduced_cost and dual are always forbidden, as they depend on the solution.
static ForbiddenNodes ForbidNonLinearNodes()
{
    ForbiddenNodes f;
    f.forbidGlobally<FunctionNodeType::reduced_cost, FunctionNodeType::dual>();
    forbidVariablesInFunctionNodes(f);
    f.parentForbidsChild<FunctionNodeType::pow, VariableNode>();
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
