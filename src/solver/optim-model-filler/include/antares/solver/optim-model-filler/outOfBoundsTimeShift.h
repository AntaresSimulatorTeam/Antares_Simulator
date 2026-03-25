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

namespace Antares::Optimisation::LinearProblemApi
{
class FillContext;
}

namespace Antares::Optimisation
{
bool hasOutOfBoundsTimeShift(const Expressions::Nodes::Node* node,
                             unsigned timeStep,
                             const LinearProblemApi::FillContext& ctx,
                             Expressions::Visitors::EvalVisitor& evalVisitor);
} // namespace Antares::Optimisation
