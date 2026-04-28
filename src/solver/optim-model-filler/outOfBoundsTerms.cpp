// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include <antares/expressions/nodes/ExpressionsNodes.h>
#include <antares/expressions/visitors/EvalVisitor.h>
#include <antares/solver/optim-model-filler/outOfBoundsTerms.h>

namespace Antares::Optimisation
{
bool hasOutOfBoundsTerms(const Expressions::Nodes::Node* node,
                         unsigned timeStep,
                         const LinearProblemApi::FillContext& ctx,
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

    if (const auto* timeSumNode = dynamic_cast<const Expressions::Nodes::TimeSumNode*>(node))
    {
        const auto from = static_cast<int>(
          evalVisitor.dispatch(timeSumNode->from()).valueAsDouble());
        const auto to = static_cast<int>(evalVisitor.dispatch(timeSumNode->to()).valueAsDouble());
        const int absoluteFrom = static_cast<int>(timeStep) + from;
        const int absoluteTo = static_cast<int>(timeStep) + to;
        if (absoluteFrom < static_cast<int>(ctx.getLocalFirstTimeStep())
            || absoluteTo > static_cast<int>(ctx.getLocalLastTimeStep()))
        {
            return true;
        }
    }

    if (const auto* parentNode = dynamic_cast<const Expressions::Nodes::ParentNode*>(node))
    {
        for (const auto* operand: parentNode->getConstOperands())
        {
            if (hasOutOfBoundsTerms(operand, timeStep, ctx, evalVisitor))
            {
                return true;
            }
        }
    }

    return false;
}
} // namespace Antares::Optimisation
