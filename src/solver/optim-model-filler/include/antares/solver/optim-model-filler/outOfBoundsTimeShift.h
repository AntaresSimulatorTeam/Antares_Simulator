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

namespace Antares::ModelerStudy::SystemModel
{
class Component;
}

namespace Antares::LinearProblem::Api
{
class FillContext;
}

namespace Antares::LinearProblem
{
/**
 * @brief Tells whether evaluating @p node at @p timeStep requires a time shift out of bounds.
 *
 * The no-connection expression of a PortFieldSumNode is only inspected when the port is not
 * connected in @p component, mirroring what the expression visitors evaluate.
 */
bool hasOutOfBoundsTimeShift(const Expressions::Nodes::Node* node,
                             const ModelerStudy::SystemModel::Component& component,
                             unsigned timeStep,
                             const Api::FillContext& ctx,
                             Expressions::Visitors::EvalVisitor& evalVisitor);
} // namespace Antares::LinearProblem
