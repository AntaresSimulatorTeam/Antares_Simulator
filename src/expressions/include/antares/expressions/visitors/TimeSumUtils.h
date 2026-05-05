// Copyright 2007-2026, RTE
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include "antares/expressions/nodes/ExpressionsNodes.h"

namespace Antares::Expressions::Visitors
{
class EvalVisitor; // forward declaration to avoid include cycles
class VariabilityVisitor;

// Normalize a possibly negative or out-of-range time index into [0, size).
int normalizeTimeIndex(int timeIndex, int size);

template<class Callback>
inline void forEachTimeSumIndex(int from, int to, int size, Callback callback)
{
    for (int timeIndex = from; timeIndex <= to; ++timeIndex)
    {
        callback(normalizeTimeIndex(timeIndex, size));
    }
}

// Resolve a bound used in a time-sum when no variability check is required.
int resolveTimeSumBound(const Nodes::Node* bound, EvalVisitor& visitor, unsigned localTimeStep);

// Resolve a bound used in a time-sum while verifying the bound is time-constant.
int resolveTimeSumBound(const Nodes::Node* bound,
                        EvalVisitor& visitor,
                        VariabilityVisitor& variabilityVisitor,
                        unsigned localTimeStep);

} // namespace Antares::Expressions::Visitors
