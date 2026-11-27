// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include <algorithm>

#include <antares/expressions/nodes/ExpressionsNodes.h>
#include <antares/expressions/visitors/EvalVisitor.h>
#include <antares/expressions/visitors/TimeSumUtils.h>
#include <antares/solver/optim-model-filler/outOfBoundsTimeShift.h>
#include <antares/study/system-model/component.h>
#include <antares/study/system-model/connection.h>

namespace Antares::LinearProblem
{
bool hasOutOfBoundsTimeShift(const Expressions::Nodes::Node* node,
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
        // Fractional shifts are truncated toward zero (e.g., 1.5 → 1, -1.5 → -1).
        const auto timeShift = static_cast<int>(
          evalVisitor.dispatch(timeShiftNode->right()).valueAsDouble());
        const int shiftedTimestep = static_cast<int>(timeStep) + timeShift;
        if (shiftedTimestep < static_cast<int>(ctx.getLocalFirstTimeStep())
            || shiftedTimestep > static_cast<int>(ctx.getLocalLastTimeStep()))
        {
            return true;
        }
        // Shifts compose: the shifted operand is evaluated at the shifted time step
        return hasOutOfBoundsTimeShift(timeShiftNode->left(),
                                       static_cast<unsigned>(shiftedTimestep),
                                       ctx,
                                       evalVisitor)
               || hasOutOfBoundsTimeShift(timeShiftNode->right(), timeStep, ctx, evalVisitor);
    }

    if (const auto* portFieldSumNode = dynamic_cast<const Expressions::Nodes::PortFieldSumNode*>(
          node))
    {
        const auto& connectionEnds = evalVisitor.component().componentConnectionsViaPort(
          portFieldSumNode->getPortName());
        const bool peerOutOfBounds = std::ranges::any_of(
          connectionEnds,
          [&](const ModelerStudy::SystemModel::ConnectionEnd& connectionEnd)
          {
              const auto& peer = connectionEnd.component();
              auto peerVisitor = evalVisitor.forComponent(peer);
              const auto* peerExpression = peer.nodeAtPortField(connectionEnd.port().Id(),
                                                                portFieldSumNode->getFieldName());
              return hasOutOfBoundsTimeShift(peerExpression, timeStep, ctx, peerVisitor);
          });
        if (peerOutOfBounds)
        {
            return true;
        }
    }

    if (const auto* timeSumNode = dynamic_cast<const Expressions::Nodes::TimeSumNode*>(node))
    {
        // The window [from, to] is expressed in local indices, relative to the local time step
        // for "t + N" bounds. An empty window (from > to) adds no term.
        const auto localFirst = static_cast<long long>(ctx.getLocalFirstTimeStep());
        const auto localLast = static_cast<long long>(ctx.getLocalLastTimeStep());
        const auto localTimeStep = static_cast<unsigned>(static_cast<long long>(timeStep)
                                                         - localFirst);
        const long long from = Expressions::Visitors::resolveTimeSumBound(timeSumNode->from(),
                                                                          evalVisitor,
                                                                          localTimeStep);
        const long long to = Expressions::Visitors::resolveTimeSumBound(timeSumNode->to(),
                                                                        evalVisitor,
                                                                        localTimeStep);
        if (from > to)
        {
            return false;
        }
        if (from < 0 || to > localLast - localFirst)
        {
            return true;
        }
        // The window is inside the block (at most one step per time step of the block): the
        // summed expression is evaluated at each time step of the window.
        for (long long index = from; index <= to; ++index)
        {
            if (hasOutOfBoundsTimeShift(timeSumNode->expression(),
                                        static_cast<unsigned>(localFirst + index),
                                        ctx,
                                        evalVisitor))
            {
                return true;
            }
        }
        return false;
    }

    if (const auto* parentNode = dynamic_cast<const Expressions::Nodes::ParentNode*>(node))
    {
        for (const auto* operand: parentNode->getConstOperands())
        {
            if (hasOutOfBoundsTimeShift(operand, timeStep, ctx, evalVisitor))
            {
                return true;
            }
        }
    }

    return false;
}
} // namespace Antares::LinearProblem
