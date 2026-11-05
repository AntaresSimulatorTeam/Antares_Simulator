// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include <antares/expressions/nodes/ExpressionsNodes.h>
#include <antares/expressions/visitors/EvalVisitor.h>
#include <antares/expressions/visitors/TimeSumUtils.h>
#include <antares/solver/optim-model-filler/outOfBoundsTimeShift.h>

namespace Antares::LinearProblem
{
namespace
{
bool isOutsideBlock(int localTimeStep, int numberOfTimeSteps)
{
    return localTimeStep < 0 || localTimeStep >= numberOfTimeSteps;
}

// A relative time-sum is evaluated at timeStep. Its window is out of bounds when a relative bound
// leaves the block; otherwise its operand is checked at every timestep the window selects.
bool hasOutOfBoundsRelativeTimeSum(const Expressions::Nodes::TimeSumNode* timeSumNode,
                                   unsigned timeStep,
                                   const Api::FillContext& ctx,
                                   Expressions::Visitors::EvalVisitor& evalVisitor)
{
    using Expressions::Nodes::TPlusNode;
    const auto firstTimeStep = ctx.getLocalFirstTimeStep();
    const auto localTimeStep = timeStep - firstTimeStep;
    const auto numberOfTimeSteps = static_cast<int>(ctx.getLocalNumberOfTimeSteps());
    const auto from = Expressions::Visitors::resolveTimeSumBound(timeSumNode->from(),
                                                                 evalVisitor,
                                                                 localTimeStep,
                                                                 numberOfTimeSteps);
    const auto to = Expressions::Visitors::resolveTimeSumBound(timeSumNode->to(),
                                                               evalVisitor,
                                                               localTimeStep,
                                                               numberOfTimeSteps);
    if (from > to)
    {
        return false; // Empty window: the operand is never evaluated
    }

    const bool fromRelative = dynamic_cast<const TPlusNode*>(timeSumNode->from()) != nullptr;
    const bool toRelative = dynamic_cast<const TPlusNode*>(timeSumNode->to()) != nullptr;
    if ((fromRelative && isOutsideBlock(from, numberOfTimeSteps))
        || (toRelative && isOutsideBlock(to, numberOfTimeSteps)))
    {
        return true;
    }

    // Absolute bounds are already checked to be inside the block, so is the whole window
    for (int index = from; index <= to; ++index)
    {
        if (hasOutOfBoundsTimeShift(timeSumNode->expression(),
                                    firstTimeStep + static_cast<unsigned>(index),
                                    ctx,
                                    evalVisitor))
        {
            return true;
        }
    }
    return false;
}
} // namespace

bool hasOutOfBoundsTimeShift(const Expressions::Nodes::Node* node,
                             unsigned timeStep,
                             const Api::FillContext& ctx,
                             Expressions::Visitors::EvalVisitor& evalVisitor)
{
    if (!node)
    {
        return false;
    }

    // Operators that collapse time are not tied to the constraint instance's timestep: the time
    // shifts and windows they contain wrap cyclically over the block.
    if (dynamic_cast<const Expressions::Nodes::AllTimeSumNode*>(node)
        || dynamic_cast<const Expressions::Nodes::TimeIndexNode*>(node))
    {
        return false;
    }

    if (const auto* timeSumNode = dynamic_cast<const Expressions::Nodes::TimeSumNode*>(node))
    {
        if (Expressions::Visitors::hasAbsoluteBounds(timeSumNode))
        {
            return false;
        }
        return hasOutOfBoundsRelativeTimeSum(timeSumNode, timeStep, ctx, evalVisitor);
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
        // The shifted operand is evaluated at the shifted timestep
        return hasOutOfBoundsTimeShift(timeShiftNode->left(),
                                       static_cast<unsigned>(shiftedTimestep),
                                       ctx,
                                       evalVisitor);
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
