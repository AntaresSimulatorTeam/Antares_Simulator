// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include <antares/expressions/nodes/ExpressionsNodes.h>
#include <antares/expressions/visitors/EvalVisitor.h>
#include <antares/solver/optim-model-filler/outOfBoundsTimeShift.h>
#include <antares/study/system-model/component.h>

namespace Antares::LinearProblem
{
bool hasOutOfBoundsTimeShift(const Expressions::Nodes::Node* node,
                             const ModelerStudy::SystemModel::Component& component,
                             unsigned timeStep,
                             const Api::FillContext& ctx,
                             Expressions::Visitors::EvalVisitor& evalVisitor)
{
    if (!node)
    {
        return false;
    }

    if (const auto* timeShiftNode = dynamic_cast<const Expressions::Nodes::TimeShiftNode*>(node))
    {
        const auto timeShift = static_cast<int>(
          evalVisitor.dispatch(timeShiftNode->right()).valueAsDouble());
        const int shiftedTimestep = static_cast<int>(timeStep) + timeShift;
        if (shiftedTimestep < static_cast<int>(ctx.getLocalFirstTimeStep())
            || shiftedTimestep > static_cast<int>(ctx.getLocalLastTimeStep()))
        {
            return true;
        }
    }

    if (const auto* portFieldSumNode = dynamic_cast<const Expressions::Nodes::PortFieldSumNode*>(
          node))
    {
        // The fallback operand is only evaluated when the port is not connected
        return !component.isPortConnected(portFieldSumNode->getPortName())
               && hasOutOfBoundsTimeShift(portFieldSumNode->noConnectionExpr(),
                                          component,
                                          timeStep,
                                          ctx,
                                          evalVisitor);
    }

    if (const auto* parentNode = dynamic_cast<const Expressions::Nodes::ParentNode*>(node))
    {
        for (const auto* operand: parentNode->getConstOperands())
        {
            if (hasOutOfBoundsTimeShift(operand, component, timeStep, ctx, evalVisitor))
            {
                return true;
            }
        }
    }

    return false;
}
} // namespace Antares::LinearProblem
