// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include "antares/expressions/nodes/ExpressionsNodes.h"

namespace Antares::Expressions::Visitors
{
class EvalVisitor; // forward declaration to avoid include cycles
class VariabilityVisitor;

// Normalize a possibly negative or out-of-range time index into [0, size).
int normalizeTimeIndex(int timeIndex, int size);

// no-op if from > to
template<class Callback>
inline void forEachTimeSumIndex(int from, int to, int size, Callback callback)
{
    for (int timeIndex = from; timeIndex <= to; ++timeIndex)
    {
        callback(normalizeTimeIndex(timeIndex, size));
    }
}

// A time-sum with two absolute bounds (no t+k) collapses time: it has the same value at every
// timestep of the block.
bool hasAbsoluteBounds(const Nodes::TimeSumNode* node);

// Resolve a bound used in a time-sum when no variability check is required.
int resolveTimeSumBound(const Nodes::Node* bound,
                        EvalVisitor& visitor,
                        unsigned localTimeStep,
                        int numberOfTimeSteps);

// Check that a time-sum bound is fixed in time and can be evaluated before optimization.
void checkTimeSumBoundVariability(const Nodes::Node* bound, VariabilityVisitor& variabilityVisitor);

// Resolve the index of a TimeIndexNode (x[i]): the index must be fixed in time and resolve to an
// integer. Range checking is left to the caller, which knows the size of the indexed expression.
int resolveTimeIndex(const Nodes::Node* index,
                     EvalVisitor& visitor,
                     VariabilityVisitor& variabilityVisitor);

} // namespace Antares::Expressions::Visitors
