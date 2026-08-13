// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

namespace Antares::Expressions
{
namespace Nodes
{
class Node;
}

namespace Visitors
{
class EvalVisitor;
}
} // namespace Antares::Expressions

namespace Antares::LinearProblem::Api
{
class FillContext;
}

namespace Antares::LinearProblem
{
bool hasOutOfBoundsTimeShift(const Expressions::Nodes::Node* node,
                             unsigned timeStep,
                             const Api::FillContext& ctx,
                             Expressions::Visitors::EvalVisitor& evalVisitor);
} // namespace Antares::LinearProblem
