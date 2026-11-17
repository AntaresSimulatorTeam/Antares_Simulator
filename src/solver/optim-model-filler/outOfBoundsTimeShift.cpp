// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include <algorithm>

#include <antares/expressions/nodes/ExpressionsNodes.h>
#include <antares/expressions/visitors/EvalVisitor.h>
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
